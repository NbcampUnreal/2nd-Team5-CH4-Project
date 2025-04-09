// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GS_LobbyState.h"
#include "GM_LobbyMode.generated.h"

// 전투 맵 종류를 Enum으로 정의(맵 추가하면됨)
UENUM(BlueprintType)
enum class EBattleMapType : uint8
{
	Battlefield_01 UMETA(DisplayName = "TestMap"),
	Battlefield_02 UMETA(DisplayName = "TestMap")
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
	virtual void Tick(float DeltaSeconds) override;

protected:
	void TryStartBattle();
	FString GetMapPathFromEnum(EBattleMapType MapType) const;
	EBattleMapType GetRandomMapEnum() const;
	void StartBattle();
	void UpdateCountdown(float DeltaTime);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	int32 MinPlayersToStart = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	float BattleStartDelay = 10.0f;

private:
	bool bMatchStarted = false;
	bool bStartCountdownStarted = false;
	float CountdownRemaining = 0.0f;
};

