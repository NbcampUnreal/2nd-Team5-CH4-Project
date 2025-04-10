// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Library/InformationChck.h"

#include "GameModes/GI_BattleInstance.h"
#include "Kismet/GameplayStatics.h"

void UInformationChck::Server_CheckNickname_Implementation(UObject* WorldContextObject, const FString& Nickname)
{
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
	{
		const bool bAvailable = !GI->IsNicknameTaken(Nickname);
	}
}

void UInformationChck::Server_RegisterNickname_Implementation(UObject* WorldContextObject, const FString& Nickname)
{
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
	{
		//기본값 나이트로 일단 등록
		GI->RegisterPlayerInfo(Nickname, ECharacterType::Knight);
	}
}

void UInformationChck::Server_RegisterCharacterType_Implementation(UObject* WorldContextObject, const FString& Nickname, ECharacterType CharacterType)
{
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
	{
		GI->UpdateCharacterType(Nickname, CharacterType);;
	}
}

ECharacterType UInformationChck::GetCharacterTypeByNickname(UObject* WorldContextObject, const FString& Nickname)
{
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
	{
		return GI->GetCharacterTypeByNickname(Nickname);
	}
	return ECharacterType::Knight; // 기본값
}


