// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_LobbyMode.generated.h"

// 사용할 전투 맵 종류 정의
UENUM(BlueprintType)
enum class EGameMapType : uint8
{
	MarioMap	UMETA(DisplayName = "Mario Map"),
	// 추후 확장
};

UENUM(BlueprintType)
enum class ELobbyState : uint8
{
	Waiting,
	Countdown,
	Started
};


UCLASS()
class TEAM05_API AGM_LobbyMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGM_LobbyMode();

	// 플레이어 접속 처리
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	// 매 틱마다 TryStartBattle 및 Countdown 갱신
	virtual void BeginPlay() override;

	// 캐릭터 스폰
	void SpawnPlayerInLobby(APlayerController* PC);

protected:
	// 전투 시작 조건 확인 (모든 플레이어 Ready 여부)
	void TryStartBattle();

	// 카운트다운 시간 갱신
	void UpdateCountdown(float DeltaTime);

	// 실제로 전투 맵으로 이동
	void StartGame();
	
private:
	// 상태 확인 함수
	void CheckLobbyState();

protected:
	/** 전투 시작까지 대기 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	float BattleStartDelay;

	/** 최소 시작 인원 수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	int32 MinPlayersToStart;

	/** 카운트다운 여부 */
	UPROPERTY(VisibleAnywhere, Category = "Battle")
	bool bStartCountdownStarted;

	/** 게임이 시작되었는지 여부 */
	UPROPERTY(VisibleAnywhere, Category = "Battle")
	bool bMatchStarted;

	/** 남은 카운트다운 시간 */
	UPROPERTY(VisibleAnywhere, Category = "Battle")
	float CountdownRemaining;

	/** 전투 맵 Enum → 이름 맵핑용 테이블 */
	UPROPERTY(EditDefaultsOnly, Category = "Battle")
	TMap<EGameMapType, FString> BattleMapNames;

	/** 현재 선택된 전투 맵 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	EGameMapType SelectedBattleMap;

	// 상태 체크 주기용 타이머 핸들
	FTimerHandle LobbyStateTimerHandle;
};

