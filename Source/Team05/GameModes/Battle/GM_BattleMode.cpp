// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Battle/GM_BattleMode.h"

#include "GameModes/Battle/GS_BattleState.h"
#include "GameModes/Battle/PS_PlayerState.h"
#include "Character/MyPlayerController.h"
#include "Kismet/GameplayStatics.h"


AGM_BattleMode::AGM_BattleMode()
{
	// SeamlessTravel 활성화
	bUseSeamlessTravel = true;
}

void AGM_BattleMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (HasAuthority())
	{
		FString PlayerName = NewPlayer->GetName();

		if (AGS_BattleState* GS = GetGameState<AGS_BattleState>())
		{
			const FString Msg = FString::Printf(TEXT("%s Welcome!"), *PlayerName);
			GS->Multicast_PrintMessage(Msg);
			AMyPlayerController* NewPC = Cast<AMyPlayerController>(NewPlayer);
			if (IsValid(NewPC))
			{
				AlivePlayerControllers.Add(NewPC);
			}
		}
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
	}
}

void AGM_BattleMode::OnCharacterDead(AMyPlayerController* InController)
{
	if (!IsValid(InController) || !AlivePlayerControllers.Contains(InController))
		return;

	AlivePlayerControllers.Remove(InController);
	DeadPlayerControllers.Add(InController);
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
		UE_LOG(LogTemp, Log, TEXT("[BattleMode] 상태: Waiting - 대기 중"));
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
			UE_LOG(LogTemp, Warning, TEXT("[BattleMode] 전투 종료 - 플레이어 타이틀로 이동"));

			for (auto PC : AlivePlayerControllers) PC->ReturnToTitle();
			for (auto PC : DeadPlayerControllers) PC->ReturnToTitle();

			MainTimerHandle.Invalidate();

			// 레벨 리셋 (Dedicated 서버용)
			FName LevelName = FName("DevLobby");
			UGameplayStatics::OpenLevel(this, LevelName, true, TEXT("listen"));

			UE_LOG(LogTemp, Warning, TEXT("[BattleMode] 레벨 초기화 및 DevLobby로 이동"));
		}
		break;
	}
	default:
		break;
	}
}
