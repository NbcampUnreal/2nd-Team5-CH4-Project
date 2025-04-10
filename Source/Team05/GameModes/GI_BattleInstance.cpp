// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_BattleInstance.h"

bool UGI_BattleInstance::IsNicknameTaken(const FString& Nickname) const
{
	return PlayerInfoMap.Contains(Nickname);
}

void UGI_BattleInstance::RegisterPlayerInfo(const FString& Nickname, ECharacterType Type)
{
	if (!Nickname.IsEmpty())
	{
		PlayerInfoMap.Add(Nickname, FPlayerInfo(Nickname, Type));
	}
}

void UGI_BattleInstance::UpdateCharacterType(const FString& Nickname, ECharacterType NewType)
{
	if (PlayerInfoMap.Contains(Nickname))
	{
		PlayerInfoMap[Nickname].CharacterType = NewType;
	}
}

ECharacterType UGI_BattleInstance::GetCharacterTypeByNickname(const FString& Nickname) const
{
	if (const FPlayerInfo* Info = PlayerInfoMap.Find(Nickname))
	{
		return Info->CharacterType;
	}
	return ECharacterType::Knight; // 기본값: 등록되지 않은 경우
}
