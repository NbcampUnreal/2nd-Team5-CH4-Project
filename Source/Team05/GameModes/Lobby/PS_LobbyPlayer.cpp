// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Lobby/PS_LobbyPlayer.h"

#include "Net/UnrealNetwork.h"

APS_LobbyPlayer::APS_LobbyPlayer()
{
	bReplicates = true;

	Nickname = TEXT("Player");
	CharacterType = ECharacterType::Knight;
	bReady = false;
}

void APS_LobbyPlayer::SetNickname(const FString& NewNickname)
{
	Nickname = NewNickname;
}

FString APS_LobbyPlayer::GetNickname() const
{
	return Nickname;
}

void APS_LobbyPlayer::SetCharacterType(ECharacterType NewType)
{
	CharacterType = NewType;
}

ECharacterType APS_LobbyPlayer::GetCharacterType() const
{
	return CharacterType;
}

void APS_LobbyPlayer::SetReady(bool bNewReady)
{
	bReady = bNewReady;
}

bool APS_LobbyPlayer::IsReady() const
{
	return bReady;
}

void APS_LobbyPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS_LobbyPlayer, Nickname);
	DOREPLIFETIME(APS_LobbyPlayer, CharacterType);
	DOREPLIFETIME(APS_LobbyPlayer, bReady);
}

