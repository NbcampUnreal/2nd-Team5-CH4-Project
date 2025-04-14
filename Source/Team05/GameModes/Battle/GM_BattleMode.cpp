// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Battle/GM_BattleMode.h"

#include "GameModes/Battle/GS_BattleState.h"
#include "GameModes/Battle/PS_PlayerState.h"
#include "Character/MyPlayerController.h"
#include "GameModes/GI_BattleInstance.h"
#include "Level/Spawn.h"
#include "Kismet/GameplayStatics.h"


AGM_BattleMode::AGM_BattleMode()
{
	// SeamlessTravel 활성화
	bUseSeamlessTravel = true;
}

void AGM_BattleMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AGS_BattleState* GS = GetGameState<AGS_BattleState>();
	if (IsValid(GS) == false)
	{
		return;
	}

	if (GS->MatchState != EMatchState::Waiting)
	{
		AMyPlayerController* PC = Cast<AMyPlayerController>(NewPlayer);
		PC->Client_KickWithMessage(TEXT("게임이 진행중이라 입장이 불가능합니다."));
		return;
	}

	// 최대 인원 초과 시 퇴장 처리 (예: 4명 제한)
	if (AlivePlayerControllers.Num() >= 4) // 또는 별도 변수 사용 가능
	{
		if (AMyPlayerController* PC = Cast<AMyPlayerController>(NewPlayer))
		{
			PC->Client_KickWithMessage(TEXT("최대 인원이 초과되어 입장이 제한됩니다."));
		}
		return;
	}

	if (HasAuthority())
	{
		FString PlayerName = NewPlayer->GetName();

		if (IsValid(GS))
		{
			const FString Msg = FString::Printf(TEXT("%s Welcome!"), *PlayerName);
			GS->Multicast_PrintMessage(Msg);
			AMyPlayerController* NewPC = Cast<AMyPlayerController>(NewPlayer);
			if (IsValid(NewPC))
			{
				AlivePlayerControllers.Add(NewPC);
			}
		}

		SpawnPlayerInBattle(NewPlayer);
	}
}

void AGM_BattleMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	AMyPlayerController* ExitingPlayerController = Cast<AMyPlayerController>(Exiting);
	if (IsValid(ExitingPlayerController) == true && AlivePlayerControllers.Find(ExitingPlayerController) != INDEX_NONE)
	{
		AlivePlayerControllers.Remove(ExitingPlayerController);
		DeadPlayerControllers.Add(ExitingPlayerController);

		// ID 가져오기
		FString ID = ExitingPlayerController->GetPlayerUniqueID();

		// GameInstance에서 정보 삭제
		if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
		{
			if (!(GetWorld() && GetWorld()->IsInSeamlessTravel()))
			{
				if (GI->PlayerInfoMap.Remove(ID) > 0)
				{
					UE_LOG(LogTemp, Log, TEXT("[Logout] Removed player info for ID: %s"), *ID);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Logout] Skipped removing player info during travel. ID: %s"), *ID);
			}
		}
	}
}


void AGM_BattleMode::OnCharacterDead(AMyPlayerController* InController)
{
	if (!IsValid(InController) || !AlivePlayerControllers.Contains(InController))
		return;

	AlivePlayerControllers.Remove(InController);
	DeadPlayerControllers.Add(InController);
}

void AGM_BattleMode::SpawnPlayerInBattle(APlayerController* Player)
{
	// 여기부터 스폰로직
		// GameInstance에서 캐릭터 정보 받아오기
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		PlayerSpawnList = GI->CachedSpawnList;
		UE_LOG(LogTemp, Log, TEXT("[Spawn] Retrieved %d player(s) from CachedSpawnList."), PlayerSpawnList.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Spawn] GameInstance cast failed."));
		return;
	}

	AMyPlayerController* MyPC = Cast<AMyPlayerController>(Player);
	if (!MyPC)
	{
		UE_LOG(LogTemp, Error, TEXT("[Spawn] Invalid PlayerController."));
		return;
	}

	const FString UniqueID = MyPC->GetPlayerUniqueID();
	if (UniqueID.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[Spawn] Failed to get Unique ID from Controller."));
		return;
	}

	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		if (!GI->PlayerInfoMap.Contains(UniqueID))
		{
			UE_LOG(LogTemp, Error, TEXT("[Spawn] ID %s not found in GameInstance."), *UniqueID);
			return;
		}

		const FPlayerInfo& Info = GI->PlayerInfoMap[UniqueID];

		if (!Info.CharacterClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[Spawn] CharacterClass is null for ID %s"), *UniqueID);
			return;
		}

		TArray<AActor*> FoundPoints;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawn::StaticClass(), FoundPoints);
		UE_LOG(LogTemp, Log, TEXT("[Spawn] Found %d spawn point(s)."), FoundPoints.Num());

		bool bSpawned = false;

		for (AActor* Actor : FoundPoints)
		{
			ASpawn* Point = Cast<ASpawn>(Actor);
			if (Point && Point->GetPlayerNum() == Info.PlayerNum)
			{
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				APawn* NewPawn = GetWorld()->SpawnActor<APawn>(
					Info.CharacterClass,
					Point->GetActorLocation(),
					Point->GetActorRotation(),
					Params
				);

				if (NewPawn)
				{
					Player->Possess(NewPawn);
					UE_LOG(LogTemp, Log, TEXT("[Spawn] ID %s: Possessed Pawn at %s"), *UniqueID, *Point->GetName());
					bSpawned = true;
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[Spawn] Failed to spawn pawn for ID %s."), *UniqueID);
				}
				break;
			}
		}

		if (!bSpawned)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Spawn] No spawn point found for PlayerNum %d"), Info.PlayerNum);
		}
	}
}

void AGM_BattleMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	if (APlayerController* PC = Cast<APlayerController>(C))
	{
		SpawnPlayerInBattle(PC); // 스폰 함수 호출

		AMyPlayerController* NewPC = Cast<AMyPlayerController>(PC);
		if (IsValid(NewPC))
		{
			AlivePlayerControllers.Add(NewPC);
		}

		// 1초 간격으로 로비 상태 확인
		GetWorldTimerManager().SetTimer(MainTimerHandle, this, &AGM_BattleMode::OnMainTimerElapsed, 1.0f, true);
	}
}

void AGM_BattleMode::OnMainTimerElapsed()
{
	// 게임 상태 가져오기
	AGS_BattleState* GS = GetGameState<AGS_BattleState>();
	if (!IsValid(GS)) return;

	switch (GS->MatchState)
	{
	case EMatchState::Waiting:
	{
		if (AlivePlayerControllers.Num() < MinimumPlayerCountForPlaying)
		{
			RemainWaitingTimeForPlaying = WaitingTime;
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[BattleMode] 상태: Waiting - Countdown"));
			--RemainWaitingTimeForPlaying;
		}

		if (RemainWaitingTimeForPlaying <= 0)
		{
			GS->MatchState = EMatchState::Playing;
		}

		break;
	}
	case EMatchState::Playing:
	{
		// 현재 생존 플레이어 수 업데이트
		GS->AlivePlayerControllerCount = AlivePlayerControllers.Num();

		UE_LOG(LogTemp, Log, TEXT("[BattleMode] 상태: Playing - 생존자 수: %d"), GS->AlivePlayerControllerCount);

		// 생존자가 1명 이하이면 게임 종료 상태로
		if (GS->AlivePlayerControllerCount <= 1)
		{
			GS->MatchState = EMatchState::Ending;
			UE_LOG(LogTemp, Warning, TEXT("[BattleMode] 생존자 1명 이하 - Ending 상태로 전환"));
		}
		break;
	}
	case EMatchState::Ending:
	{
		--RemainWaitingTimeForEnding;
		UE_LOG(LogTemp, Log, TEXT("[BattleMode] 상태: Ending - 종료까지 %d초 남음"), RemainWaitingTimeForEnding);

		if (RemainWaitingTimeForEnding <= 0)
		{
			// 모든 플레이어 타이틀로 복귀
			/*UE_LOG(LogTemp, Warning, TEXT("[BattleMode] 전투 종료 - 플레이어 타이틀로 이동"));

			for (auto PC : AlivePlayerControllers) PC->ReturnToTitle();
			for (auto PC : DeadPlayerControllers) PC->ReturnToTitle();*/

			MainTimerHandle.Invalidate();

			// 레벨 리셋 (Dedicated 서버용)
			FString LevelName = "DevLobby";
			GetWorld()->ServerTravel(LevelName + TEXT("?listen"), true);

			UE_LOG(LogTemp, Warning, TEXT("[BattleMode] 레벨 초기화 및 DevLobby로 이동"), true);
		}
		break;
	}
	default:
		break;
	}
}

void AGM_BattleMode::BeginPlay()
{
	Super::BeginPlay();

	// 1초 간격으로 로비 상태 확인
	GetWorldTimerManager().SetTimer(MainTimerHandle, this, &AGM_BattleMode::OnMainTimerElapsed, 1.0f, true);
}
