// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/Lobby/GM_LobbyMode.h"
#include "GS_LobbyState.generated.h"

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

public:

	// 현재 매치 상태 (대기/플레이/종료 등)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	ELobbyState LobbyState = ELobbyState::Waiting;

private:

	UPROPERTY(Replicated)
	float CountdownTime;
};

