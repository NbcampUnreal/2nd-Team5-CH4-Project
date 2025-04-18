// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Lobby/GS_LobbyState.h"

#include "Net/UnrealNetwork.h"

AGS_LobbyState::AGS_LobbyState()
{
	bReplicates = true;
	LobbyState = ELobbyState::Waiting;
	CountdownTime = 0.0f;
}

void AGS_LobbyState::SetLobbyState(ELobbyState NewState)
{
	LobbyState = NewState;
}

ELobbyState AGS_LobbyState::GetLobbyState() const
{
	return LobbyState;
}

void AGS_LobbyState::SetCountdownTime(float NewTime)
{
	CountdownTime = NewTime;
}

float AGS_LobbyState::GetCountdownTime() const
{
	return CountdownTime;
}

void AGS_LobbyState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, LobbyState);
	DOREPLIFETIME(ThisClass, CountdownTime);
}

