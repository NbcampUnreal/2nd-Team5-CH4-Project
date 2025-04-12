// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Lobby/GM_LobbyMode.h"

#include "GameModes/GI_BattleInstance.h"
#include "GameModes/Battle/PS_PlayerState.h"
#include "GameModes/Lobby/GS_LobbyState.h"
#include "Character/MyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

AGM_LobbyMode::AGM_LobbyMode()
{
	PrimaryActorTick.bCanEverTick = false;

	BattleStartDelay = 10.0f;
	MinPlayersToStart = 2;

	bStartCountdownStarted = false;
	bMatchStarted = false;
	CountdownRemaining = 10.0f;

	// 전투 맵 이름 등록
	BattleMapNames.Add(EGameMapType::MarioMap, TEXT("_MarioMap"));

	// 기본 맵 설정
	SelectedBattleMap = EGameMapType::MarioMap;

	// SeamlessTravel 활성화
	bUseSeamlessTravel = false;
}

void AGM_LobbyMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	/*AGS_LobbyState* GS = GetGameState<AGS_LobbyState>();
	if (IsValid(GS) == false)
	{
		return;
	}

	if (GS->LobbyState != ELobbyState::Waiting)
	{
		NewPlayer->SetLifeSpan(0.1f);
		UE_LOG(LogTemp, Log, TEXT("[Spawn] Return"));
		return;
	}*/

	// 유저 고유 ID 기반으로 GameInstance에 PlayerNum 등록
	if (APS_PlayerState* PS = Cast<APS_PlayerState>(NewPlayer->PlayerState))
	{
		if (AMyPlayerController* MPC = Cast<AMyPlayerController>(NewPlayer))
		{
			FString ID = MPC->GetPlayerUniqueID();

			if (!ID.IsEmpty())
			{
				// GameInstance 등록
				if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					FString OutNickname;
					int32 OutPlayerNum = 0;

					if (GI->RegisterPlayerID(ID, OutNickname, OutPlayerNum))
					{
						PS->SetPlayerNum(OutPlayerNum);
						UE_LOG(LogTemp, Log, TEXT("[LobbyMode] Assigned PlayerNum: %d to ID: %s"), OutPlayerNum, *ID);
					}
				}
				else {
					UE_LOG(LogTemp, Log, TEXT("[LobbyMode] GameInstance Casting Failed"));
				}
			}
			else {
				UE_LOG(LogTemp, Log, TEXT("[LobbyMode] ID Empty"));
			}
		}
		else {
			UE_LOG(LogTemp, Log, TEXT("[LobbyMode] Controller CastingFailed"));
		}
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("[LobbyMode] PlayerState CastingFailed"));
	}
}

void AGM_LobbyMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void AGM_LobbyMode::BeginPlay()
{
	Super::BeginPlay();

	// 1초 간격으로 로비 상태 확인
	GetWorldTimerManager().SetTimer(LobbyStateTimerHandle, this, &AGM_LobbyMode::CheckLobbyState, 1.0f, true);
}

// 모든 플레이어가 준비되었는지 확인하고, 카운트다운 시작 조건 체크
void AGM_LobbyMode::TryStartBattle()
{
	if (bMatchStarted || bStartCountdownStarted) return;

	int32 ReadyCount = 0;
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (const APS_PlayerState* MyPS = Cast<APS_PlayerState>(PS))
		{
			if (MyPS->IsReady())
			{
				ReadyCount++;
			}
		}
	}

	if (ReadyCount >= MinPlayersToStart && ReadyCount == GameState->PlayerArray.Num())
	{
		bStartCountdownStarted = true;
		CountdownRemaining = BattleStartDelay;

		if (AGS_LobbyState* GS = GetGameState<AGS_LobbyState>())
		{
			GS->SetLobbyState(ELobbyState::Countdown);
			GS->SetCountdownTime(CountdownRemaining);
		}

		UE_LOG(LogTemp, Warning, TEXT("[LobbyMode] All players ready. Countdown started."));
	}
}

// 카운트다운 진행 및 시간 종료 시 게임 시작
void AGM_LobbyMode::UpdateCountdown(float Time)
{
	if (!bStartCountdownStarted || bMatchStarted) return;

	CountdownRemaining -= Time;

	if (AGS_LobbyState* GS = GetGameState<AGS_LobbyState>())
	{
		GS->SetCountdownTime(FMath::Max(CountdownRemaining, 0.0f));
	}

	if (CountdownRemaining <= 0.0f)
	{
		StartGame();
	}
}

// 게임 시작 및 ServerTravel 실행
void AGM_LobbyMode::StartGame()
{
	bMatchStarted = true;

	if (AGS_LobbyState* GS = GetGameState<AGS_LobbyState>())
	{
		GS->SetLobbyState(ELobbyState::Started);
		GS->SetCountdownTime(0.0f);
	}

	// 선택된 맵 이름으로 ServerTravel
	if (BattleMapNames.Contains(SelectedBattleMap))
	{
		const FString& MapName = BattleMapNames[SelectedBattleMap];
		UE_LOG(LogTemp, Warning, TEXT("[LobbyMode] Starting battle. ServerTravel to map: %s"), *MapName);

		GetWorld()->ServerTravel(MapName + TEXT("?listen"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LobbyMode] Invalid map type selected."));
	}
}

void AGM_LobbyMode::CheckLobbyState()
{
	AGS_LobbyState* GS = GetGameState<AGS_LobbyState>();
	if (!IsValid(GS)) return;

	switch (GS->GetLobbyState())
	{
	case ELobbyState::Waiting:
		UE_LOG(LogTemp, Log, TEXT("[LobbyState] Waiting 상태입니다."));
		TryStartBattle();
		break;

	case ELobbyState::Countdown:
		UE_LOG(LogTemp, Log, TEXT("[LobbyState] Countdown 중입니다."));
		UpdateCountdown(1.0f); // Tick과 달리 고정된 주기 사용
		break;

	case ELobbyState::Started:
		UE_LOG(LogTemp, Log, TEXT("[LobbyState] 이미 게임이 시작됨."));
		break;

	default:
		break;
	}
}

// 캐릭터 선택 완료 후 호출되어 실제 캐릭터 스폰
void AGM_LobbyMode::SpawnPlayerInLobby(APlayerController* PC)
{
	if (!IsValid(PC)) return;

	if (APS_PlayerState* PS = Cast<APS_PlayerState>(PC->PlayerState))
	{
		if (AMyPlayerController* MPC = Cast<AMyPlayerController>(PC))
		{
			FString ID = MPC->GetPlayerUniqueID();

			if (!ID.IsEmpty())
			{
				if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TSubclassOf<APawn> CharacterClass = GI->GetCharacterClass(ID);

					if (CharacterClass)
					{
						AActor* StartSpot = FindPlayerStart(PC);
						FTransform SpawnTransform = StartSpot ? StartSpot->GetActorTransform() : FTransform();

						APawn* NewPawn = GetWorld()->SpawnActor<APawn>(CharacterClass, SpawnTransform);
						if (IsValid(NewPawn))
						{
							PC->Possess(NewPawn);
							UE_LOG(LogTemp, Log, TEXT("[LobbyMode] %s spawned character %s"), *PS->GetPlayerName(), *CharacterClass->GetName());
						}
					}
				}
			}
		}
	}
}

