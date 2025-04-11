// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Battle/GM_BattleMode.h"

#include "GameModes/Battle/GS_BattleState.h"
#include "GameModes/Battle/PS_PlayerState.h"
#include "Character/MyPlayerController.h"
#include "GameModes/GI_BattleInstance.h"
#include "Level/Spawn.h"
#include "Kismet/GameplayStatics.h"
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

void AGM_BattleMode::BeginPlay()
{
	Super::BeginPlay();

    if (!HasAuthority()) return;

    // GameInstance에서 캐릭터 정보 받아오기
    if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
    {
        TArray<FPlayerInfo> temp = GI->CachedSpawnList;
        PlayerSpawnList = GI->CachedSpawnList;
    }

    // 캐릭터 스폰
    TArray<AActor*> FoundPoints;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawn::StaticClass(), FoundPoints);

    for (const FPlayerInfo& Info : PlayerSpawnList)
    {
        for (AActor* Actor : FoundPoints)
        {
            ASpawn* Point = Cast<ASpawn>(Actor);
            if (Point && Point->GetPlayerNum() == Info.PlayerNum)
            {
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                GetWorld()->SpawnActor<APawn>(
                    Info.CharacterClass,
                    Point->GetActorLocation(),
                    Point->GetActorRotation(),
                    Params
                );

                break;
            }
        }
    }
	
}
