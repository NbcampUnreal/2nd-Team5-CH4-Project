// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_BattleMode.generated.h"

class AMyPlayerController;
struct FPlayerInfo;

UCLASS()
class TEAM05_API AGM_BattleMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGM_BattleMode();

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TSubclassOf<APawn> KnightCharacterClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AMyPlayerController>> AlivePlayerControllers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AMyPlayerController>> DeadPlayerControllers;

private:
	TArray<FPlayerInfo> PlayerSpawnList;

};
