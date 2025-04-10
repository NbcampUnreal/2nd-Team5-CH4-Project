// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GS_LobbyState.generated.h"

// 로비 상태를 표현하는 Enum
UENUM(BlueprintType)
enum class ELobbyState : uint8
{
	Waiting UMETA(DisplayName = "Waiting"),
	Countdown UMETA(DisplayName = "Countdown"),
	Started UMETA(DisplayName = "Started")
};

/**
 * 
 */
UCLASS()
class TEAM05_API AGS_LobbyState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AGS_LobbyState();

	// 상태 설정 함수
	void SetLobbyState(ELobbyState NewState);
	ELobbyState GetLobbyState() const;

	// 카운트다운 설정
	void SetCountdownTime(float NewTime);
	float GetCountdownTime() const;

protected:
	// 복제 대상 등록
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// 로비 현재 상태
	UPROPERTY(Replicated)
	ELobbyState LobbyState;

	// 전투 시작까지 남은 시간
	UPROPERTY(Replicated)
	float CountdownTime;
};

