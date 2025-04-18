//GM_LobbyMode.cpp

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
	//최소인원
	MinPlayersToStart = 2;

	bStartCountdownStarted = false;
	bMatchStarted = false;
	CountdownRemaining = 10.0f;

	// 전투 맵 이름 등록
	BattleMapNames.Add(EGameMapType::MarioMap, TEXT("_MarioMap"));
	BattleMapNames.Add(EGameMapType::PocketMap, TEXT("_PoketLeagueMap"));
	BattleMapNames.Add(EGameMapType::ArenaMap, TEXT("_Arena"));

	// 기본 맵 설정
	SelectedBattleMap = EGameMapType::MarioMap;

	// SeamlessTravel 활성화
	bUseSeamlessTravel = true;
}

void AGM_LobbyMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AGS_LobbyState* GS = GetGameState<AGS_LobbyState>();
	if (IsValid(GS) == false)
	{
		return;
	}

	if (GS->LobbyState != ELobbyState::Waiting)
	{
		AMyPlayerController* PC = Cast<AMyPlayerController>(NewPlayer);
		PC->Client_KickWithMessage(TEXT("게임이 진행중이라 입장이 불가능합니다."));
		return;
	}

	// 최대 인원 초과 시 퇴장 처리 (예: 4명 제한)
	if (ConnectPlayerControllers.Num() >= 4) // 또는 별도 변수 사용 가능
	{
		if (AMyPlayerController* PC = Cast<AMyPlayerController>(NewPlayer))
		{
			PC->Client_KickWithMessage(TEXT("최대 인원이 초과되어 입장이 제한됩니다."));
		}
		return;
	}

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

						// GameInstance에 저장된 닉네임이 있으면 PlayerState에 복사
						if (GI->PlayerInfoMap.Contains(ID))
						{
							FString StoredNickname = GI->PlayerInfoMap[ID].Nickname;
							if (!StoredNickname.IsEmpty())
							{
								PS->SetPlayerNickName(StoredNickname);
								UE_LOG(LogTemp, Log, TEXT("[LobbyMode] Set nickname: %s for player ID: %s"), *StoredNickname, *ID);
							}
						}

						// 플레이어 카운팅용
						FString PlayerName = NewPlayer->GetName();
						if (IsValid(GS))
						{
							const FString Msg = FString::Printf(TEXT("%s Welcome!"), *PlayerName);
							AMyPlayerController* NewPC = Cast<AMyPlayerController>(NewPlayer);
							if (IsValid(NewPC))
							{
								ConnectPlayerControllers.Add(NewPC);
							}
						}
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

	AMyPlayerController* ExitingPlayerController = Cast<AMyPlayerController>(Exiting);
	if (IsValid(ExitingPlayerController) == true && ConnectPlayerControllers.Find(ExitingPlayerController) != INDEX_NONE)
	{
		ConnectPlayerControllers.Remove(ExitingPlayerController);

		// ID 가져오기
		FString ID = ExitingPlayerController->GetPlayerUniqueID();

		// GameInstance에서 정보 삭제
		if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
		{
			if (!(GetWorld() && GetWorld()->IsInSeamlessTravel()))
			{
				if (!GI->bSingleMode)
				{
					if (GI->PlayerInfoMap.Remove(ID) > 0)
					{
						UE_LOG(LogTemp, Log, TEXT("[Logout] Removed player info for ID: %s"), *ID);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Logout] Skipped removing player info during travel. ID: %s"), *ID);
			}
		}
	}
}

void AGM_LobbyMode::BeginPlay()
{
	Super::BeginPlay();

	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		// 싱글모드면 플레이어 1명
		if (GI->bSingleMode)
		{
			MinPlayersToStart = 1;

			// 현재 플레이어 컨트롤러 가져오기
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				if (AMyPlayerController* MPC = Cast<AMyPlayerController>(PC))
				{
					FString ID = MPC->GetPlayerUniqueID();
					if (!ID.IsEmpty())
					{
						// 플레이어 상태
						if (APS_PlayerState* PS = Cast<APS_PlayerState>(MPC->PlayerState))
						{
							FString OutNickname;
							int32 OutPlayerNum = 0;

							if (GI->RegisterPlayerID(ID, OutNickname, OutPlayerNum))
							{
								PS->SetPlayerNum(OutPlayerNum);

								if (GI->PlayerInfoMap.Contains(ID))
								{
									FString StoredNickname = GI->PlayerInfoMap[ID].Nickname;
									if (!StoredNickname.IsEmpty())
									{
										PS->SetPlayerNickName(StoredNickname);
										UE_LOG(LogTemp, Log, TEXT("[LobbyMode] Set nickname: %s for player ID: %s"), *StoredNickname, *ID);
									}
								}

								// 연결 리스트 추가
								ConnectPlayerControllers.Add(MPC);
							}
						}
					}
				}
			}
		}
	}

	// 1초 간격으로 로비 상태 확인
	GetWorldTimerManager().SetTimer(LobbyStateTimerHandle, this, &AGM_LobbyMode::CheckLobbyState, 1.0f, true);
}

//plus NameTag UI Nickname 
void AGM_LobbyMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	//전투 대기시간 30초
	CountdownRemaining = 30;

	if (APlayerController* PC = Cast<APlayerController>(C))
	{
		// 닉네임 정보 복원 로직 추가
		if (APS_PlayerState* PS = Cast<APS_PlayerState>(PC->PlayerState))
		{
			if (AMyPlayerController* MPC = Cast<AMyPlayerController>(PC))
			{
				FString ID = MPC->GetPlayerUniqueID();

				if (!ID.IsEmpty())
				{
					if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
					{
						if (GI->PlayerInfoMap.Contains(ID))
						{
							// 닉네임 유지
							FString StoredNickname = GI->PlayerInfoMap[ID].Nickname;
							if (!StoredNickname.IsEmpty())
							{
								PS->SetPlayerNickName(StoredNickname);
								UE_LOG(LogTemp, Log, TEXT("[LobbyMode][SeamlessTravel] Restored nickname: %s for player ID: %s"), *StoredNickname, *ID);
							}
						}
					}
				}
			}
		}
    
		// 연결 보장
		if (!ConnectPlayerControllers.Contains(PC))
		{
			if (AMyPlayerController* NewPC = Cast<AMyPlayerController>(PC))
			{
				ConnectPlayerControllers.Add(NewPC);
			}
		}

		// 플레이어 고유 ID 얻기 (컨트롤러에서 제공한다고 가정)
		FString PlayerID = TEXT("Unknown");
		if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(PC))
		{
			PlayerID = MyPC->GetPlayerUniqueID();
		}

		// GameInstance에서 체력 정보 가져오기
		if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
		{
			if (FPlayerInfo* Info = GI->PlayerInfoMap.Find(PlayerID))
			{
				if (Info->MatchHealth <= 0)
				{
					// 체력이 0이하인 경우 → 관전자 처리
					PC->UnPossess();
					PC->ChangeState(NAME_Spectating);

					UE_LOG(LogTemp, Warning, TEXT("[LobbyMode] Player %s is eliminated and entered as spectator."), *PlayerID);
				}
				else {
					SpawnPlayerInLobby(PC);
				}
			}
		}

		if (APS_PlayerState* PS = Cast<APS_PlayerState>(PC->PlayerState))
		{
			PS->SetReady(true);
		}
	}

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

	FString NotificationString = FString::Printf(TEXT(""));
	NotificationString = FString::Printf(TEXT("CountDown : %d "), CountdownRemaining);
	NotifyToAllPlayer(NotificationString);

	CountdownRemaining -= Time;

	if (AGS_LobbyState* GS = GetGameState<AGS_LobbyState>())
	{
		GS->SetCountdownTime(FMath::Max(CountdownRemaining, 0.0f));
	}

	if (CountdownRemaining <= 0.0f)
	{
		LobbyStateTimerHandle.Invalidate();
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

	// BattleMapNames 값이 비어있는지 확인
	if (BattleMapNames.Num() > 0)
	{
		// 키 배열로 추출해서 무작위 선택
		TArray<EGameMapType> MapTypes;
		BattleMapNames.GetKeys(MapTypes);

		// 무작위 인덱스 선택
		int32 RandomIndex = FMath::RandRange(0, MapTypes.Num() - 1);
		EGameMapType SelectedMapType = MapTypes[RandomIndex];

		// 해당 키에 대한 맵 이름 가져오기
		if (FString* MapName = BattleMapNames.Find(SelectedMapType))
		{
			UE_LOG(LogTemp, Log, TEXT("[LobbyMode] 선택된 전투 맵: %s"), **MapName);

			// 서버 트래블로 이동 (Dedicated Server 기준)
			FString TravelURL = *MapName + TEXT("?listen");
			GetWorld()->ServerTravel(TravelURL, true);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LobbyMode] BattleMapNames가 비어 있습니다."));
	}
}

void AGM_LobbyMode::CheckLobbyState()
{
	AGS_LobbyState* GS = GetGameState<AGS_LobbyState>();
	if (!IsValid(GS)) return;
	FString NotificationString = FString::Printf(TEXT(""));

	switch (GS->GetLobbyState())
	{
	case ELobbyState::Waiting:
		UE_LOG(LogTemp, Log, TEXT("[LobbyState] Waiting 상태입니다."));
		TryStartBattle();
		break;

	case ELobbyState::Countdown:

		if (!bStartCountdownStarted || bMatchStarted) return;

		
		NotificationString = FString::Printf(TEXT("CountDown : %.0f "), GS->GetCountdownTime());
		NotifyToAllPlayer(NotificationString);

		CountdownRemaining --;

		if (IsValid(GS))
		{
			GS->SetCountdownTime(FMath::Max(CountdownRemaining, 0.0f));
		}

		if (CountdownRemaining <= 0.0f)
		{
			LobbyStateTimerHandle.Invalidate();
			StartGame();
		}

		UE_LOG(LogTemp, Log, TEXT("[LobbyState] Countdown 중입니다."));
		//UpdateCountdown(1.0f); // Tick과 달리 고정된 주기 사용
		break;

	case ELobbyState::Started:
		UE_LOG(LogTemp, Log, TEXT("[LobbyState] 이미 게임이 시작됨."));
		break;

	default:
		break;
	}
}

void AGM_LobbyMode::NotifyToAllPlayer(const FString& NotificationString)
{
	for (auto ConnectPlayerController : ConnectPlayerControllers)
	{
		ConnectPlayerController->NotificationText = FText::FromString(NotificationString);
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
