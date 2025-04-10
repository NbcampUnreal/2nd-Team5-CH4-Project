// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GS_LobbyState.h"
#include "GM_LobbyMode.generated.h"

// 전투 맵 종류를 Enum으로 정의(맵 추가하면됨)
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

protected:
	// 전투 시작 조건 확인
	void TryStartBattle();

	// 카운트다운 처리 (타이머 기반)
	void CountdownTick();

	// 모든 유저 게임 레디 확인
	void CheckReadyToStart();

	// 실제 전투 맵으로 이동
	void StartBattle();

	// 맵 Enum을 맵 경로로 변환
	FString GetBattleMapPath(EBattleMap Map) const;

private:
	// 게임 레디 확인
	FTimerHandle CheckReadyTimerHandle;
	// 카운트다운 관리
	FTimerHandle CountdownTimerHandle;

	// 준비 인원 확인 후 전투 시작까지 대기하는 시간
	UPROPERTY(EditDefaultsOnly)
	float BattleStartDelay = 10.0f;

	// 최소 시작 인원 수
	UPROPERTY(EditDefaultsOnly)
	int32 MinPlayersToStart = 2;

	// 상태 변수
	bool bStartCountdownStarted;
	bool bMatchStarted;
	float CountdownRemaining;
};

