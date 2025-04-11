// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GS_LobbyState.generated.h"

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

	void SetLobbyState(ELobbyState NewState);
	ELobbyState GetLobbyState() const;

	void SetCountdownTime(float NewTime);
	float GetCountdownTime() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	UPROPERTY(Replicated)
	ELobbyState LobbyState;

	UPROPERTY(Replicated)
	float CountdownTime;
};

