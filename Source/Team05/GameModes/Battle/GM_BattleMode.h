//GM_BattleMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_BattleMode.generated.h"

// 게임 상태 열거형 - 매치의 흐름을 제어
UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None,
	Waiting,
	Playing,
	Ending,
	End
};

class AMyPlayerController;
class ABaseAIController;
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
	// SeamlessTravel
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	// 캐릭터 사망 처리 (컨트롤러 기준)
	void OnCharacterDead(AMyPlayerController* InController);

	// AI사망처리
	void OnAIDead(ABaseAIController* InAIController);

	void SpawnPlayerInBattle(APlayerController* Player);

	// AI스폰 함수
	void SpawnRandomAIs();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TriggerReadyGo();

private:
	// 1초 간격으로 호출되는 메인 타이머 콜백
	UFUNCTION()
	void OnMainTimerElapsed();

	// 싱글모드용 타이머
	UFUNCTION()
	void OnSingleTimerElapsed();


public:
	// 타이머 핸들
	FTimerHandle MainTimerHandle;
	FTimerHandle SingleTimerHandle;
	FTimerHandle EndTimerHandle;

	// 대기 시간 및 남은 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime = 5;
	int32 RemainWaitingTimeForPlaying = 5;

	// 게임 종료 대기 시간 및 남은 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EndingTime = 20;
	int32 RemainWaitingTimeForEnding = 20;

protected:

	// Player 카운팅
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AMyPlayerController>> AlivePlayerControllers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AMyPlayerController>> DeadPlayerControllers;

	//AI카운팅
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<ABaseAIController>> AliveAIControllers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<ABaseAIController>> DeadAIControllers;

	//AI 클래스들
	//컨트롤러
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<ABaseAIController> AIControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = "SingleMode|AI")
	TSubclassOf<APawn> BP_KnightAIClass;

	UPROPERTY(EditDefaultsOnly, Category = "SingleMode|AI")
	TSubclassOf<APawn> BP_MageAIClass;

	UPROPERTY(EditDefaultsOnly, Category = "SingleMode|AI")
	TSubclassOf<APawn> BP_BarbarianAIClass;

	UPROPERTY(EditDefaultsOnly, Category = "SingleMode|AI")
	TSubclassOf<APawn> BP_RogueAIClass;

private:
	TArray<FPlayerInfo> PlayerSpawnList;

	// 최소 시작 인원
	int32 MinimumPlayerCountForPlaying = 2;

};
