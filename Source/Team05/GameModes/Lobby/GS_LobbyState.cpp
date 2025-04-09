// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Lobby/GS_LobbyState.h"

#include "Net/UnrealNetwork.h"

AGS_LobbyState::AGS_LobbyState()
{
	bReplicates = true;
	CountdownTime = 0.0f;
	LobbyState = ELobbyState::Waiting;
}

void AGS_LobbyState::SetLobbyState(ELobbyState NewState)
{
	LobbyState = NewState;
}

void AGS_LobbyState::SetCountdownTime(float Time)
{
	CountdownTime = Time;
}

void AGS_LobbyState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGS_LobbyState, CountdownTime);
	DOREPLIFETIME(AGS_LobbyState, LobbyState);
}

