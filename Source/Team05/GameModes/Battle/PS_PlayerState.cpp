// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerState.h"

#include "GameModes/GI_BattleInstance.h"
#include "Net/UnrealNetwork.h"

APS_PlayerState::APS_PlayerState()
{
	CharacterType = ECharacterType2::Knight;
}

void APS_PlayerState::SetCharacterType(ECharacterType2 InType)
{
	CharacterType = InType;
}

ECharacterType2 APS_PlayerState::GetCharacterType() const
{
	return CharacterType;
}

void APS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS_PlayerState, CharacterType);
}


