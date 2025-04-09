// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Battle/GM_BattleMode.h"

#include "GameModes/Battle/GS_BattleState.h"
#include "GameModes/Battle/PS_PlayerState.h"
#include "Character/MyPlayerController.h"
//debug


AGM_BattleMode::AGM_BattleMode()
{
	test = 0;
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

	// 테스트용: Knight 캐릭터 강제 스폰
	//APS_PlayerState* PS = Cast<APS_PlayerState>(NewPlayer->PlayerState);
	//if (PS)
	//{
	//	TSubclassOf<APawn> KnightClass = KnightCharacterClass;
	//	if (KnightClass)
	//	{
	//		FVector SpawnLocation = FVector::ZeroVector; // TODO: 스폰 위치 조정
	//		FRotator SpawnRotation = FRotator::ZeroRotator;
	//		APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(KnightClass, SpawnLocation, SpawnRotation);
	//		if (SpawnedPawn)
	//		{
	//			NewPlayer->Possess(SpawnedPawn);
	//			UE_LOG(LogTemp, Log, TEXT("Knight Spawn And Posses Complete"));
	//		}
	//	}
	//}
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
