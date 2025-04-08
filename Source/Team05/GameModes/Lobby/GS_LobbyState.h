// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GS_LobbyState.generated.h"

// 로비의 상태를 정의하는 Enum
UENUM(BlueprintType)
enum class ELobbyState : uint8
{
	Waiting,
	Countdown,
	Transitioning
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

	// 남은 카운트다운 시간 반환
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	float GetCountdownTime() const { return CountdownTime; }

	// 로비 상태 반환
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	ELobbyState GetLobbyState() const { return LobbyState; }

	// 로비 상태 설정
	void SetLobbyState(ELobbyState NewState);
	void SetCountdownTime(float Time);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// 전투 시작까지 남은 시간 (클라이언트 UI 표시용)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby", meta = (AllowPrivateAccess = "true"))
	float CountdownTime;

	// 로비의 현재 상태
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby", meta = (AllowPrivateAccess = "true"))
	ELobbyState LobbyState;
};

