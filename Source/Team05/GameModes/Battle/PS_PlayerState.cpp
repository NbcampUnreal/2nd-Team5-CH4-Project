// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

APS_PlayerState::APS_PlayerState()
{
	bReplicates = true;
	PlayerNum = 0;
	Nickname = TEXT("Player");
	CharacterClass = nullptr;
	bReady = false;
}

void APS_PlayerState::SetReady(bool bInReady)
{
	bReady = true;
}

bool APS_PlayerState::IsReady() const
{
	return bReady;
}

void APS_PlayerState::SetPlayerNum(int32 InNum)
{
	PlayerNum = InNum;
}

int32 APS_PlayerState::GetPlayerNum() const
{
	return PlayerNum;
}

void APS_PlayerState::SetPlayerNickName(FString InNickname)
{
	Nickname = InNickname;
}

FString APS_PlayerState::GetPlayerNickName() const
{
	return Nickname;
}

void APS_PlayerState::SetCharacterClass(TSubclassOf<APawn> InClass)
{
	CharacterClass = InClass;
}

TSubclassOf<APawn> APS_PlayerState::GetCharacterClass() const
{
	return CharacterClass;
}

void APS_PlayerState::SetMatchHealth(int32 NewHealth)
{
	MatchHealth = NewHealth;
}

int32 APS_PlayerState::GetMatchHealth() const
{
	return MatchHealth;
}


void APS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS_PlayerState, PlayerNum);
	DOREPLIFETIME(APS_PlayerState, Nickname);
	DOREPLIFETIME(APS_PlayerState, CharacterClass);
	DOREPLIFETIME(APS_PlayerState, MatchHealth);
	DOREPLIFETIME(APS_PlayerState, bReady);
}

