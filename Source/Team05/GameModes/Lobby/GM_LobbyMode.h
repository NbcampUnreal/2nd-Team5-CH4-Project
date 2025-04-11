// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GS_LobbyState.h"
#include "GM_LobbyMode.generated.h"

UENUM(BlueprintType)
enum class EBattleMap : uint8
{
	Battlefield_01 UMETA(DisplayName = "_MarioMap"),
	Battlefield_02 UMETA(DisplayName = "_MarioMap")
};

/**
 * 
 */
UCLASS()
class TEAM05_API AGM_LobbyMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGM_LobbyMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	// 플레이어 캐릭터 스폰 함수
	UFUNCTION()
	void SpawnPlayerInLobby(APlayerController* PC);

protected:
	
	void TryStartBattle();

	void CountdownTick();

	void CheckReadyToStart();

	void StartBattle();

	FString GetBattleMapPath(EBattleMap Map) const;

private:
	
	FTimerHandle CheckReadyTimerHandle;
	
	FTimerHandle CountdownTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float BattleStartDelay = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 MinPlayersToStart = 2;

	bool bStartCountdownStarted;
	bool bMatchStarted;
	float CountdownRemaining;
};

