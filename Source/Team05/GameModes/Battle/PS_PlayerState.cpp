// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerState.h"

#include "GameModes/GI_BattleInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

APS_PlayerState::APS_PlayerState()
{

	bReplicates = true;
	Nickname = TEXT("Player");
	CharacterType = ECharacterType::Knight;
	bReady = false;
}

void APS_PlayerState::SetNickname(const FString& NewNickname)
{
	Nickname = NewNickname;
}

FString APS_PlayerState::GetNickname() const
{
	return Nickname;
}

void APS_PlayerState::SetCharacterType(ECharacterType NewType)
{
	CharacterType = NewType;
}

ECharacterType APS_PlayerState::GetCharacterType() const
{
	return CharacterType;
}

void APS_PlayerState::RegisterToGameInstance()
{
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		GI->UpdateCharacterType(Nickname, CharacterType);
		UE_LOG(LogTemp, Log, TEXT("[PlayerState] Info Registered to GameInstance: %s, %s"),
			*Nickname,
			*UEnum::GetValueAsString(CharacterType));
	}
}


void APS_PlayerState::SetReady(bool bNewReady)
{
	bReady = bNewReady;
}

bool APS_PlayerState::IsReady() const
{
	return bReady;
}

void APS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS_PlayerState, Nickname);
	DOREPLIFETIME(APS_PlayerState, CharacterType);
	DOREPLIFETIME(APS_PlayerState, bReady);
}


