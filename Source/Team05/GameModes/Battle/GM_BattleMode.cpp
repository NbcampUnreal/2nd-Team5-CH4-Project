// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Battle/GM_BattleMode.h"

#include "GameModes/Battle/GS_BattleState.h"
#include "GameModes/Battle/PS_PlayerState.h"
#include "Character/MyPlayerController.h"
//debug


AGM_BattleMode::AGM_BattleMode()
{
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
