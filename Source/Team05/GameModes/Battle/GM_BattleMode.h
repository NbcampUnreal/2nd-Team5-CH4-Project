// Fill out your copyright notice in the Description page of Project Settings.

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

/**
 * 
 */
UCLASS()
class TEAM05_API AGM_BattleMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGM_BattleMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	// 캐릭터 사망 처리 (컨트롤러 기준)
	void OnCharacterDead(AMyPlayerController* InController);

private:
	// 1초 간격으로 호출되는 메인 타이머 콜백
	UFUNCTION()
	void OnMainTimerElapsed();

public:
	// 타이머 핸들
	FTimerHandle MainTimerHandle;

	// 게임 종료 대기 시간 및 남은 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EndingTime = 15;
	int32 RemainWaitingTimeForEnding = 15;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TSubclassOf<APawn> KnightCharacterClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AMyPlayerController>> AlivePlayerControllers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AMyPlayerController>> DeadPlayerControllers;

};
