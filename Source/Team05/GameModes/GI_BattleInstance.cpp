// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_BattleInstance.h"

bool UGI_BattleInstance::TryRegisterNickname(const FString& Nickname)
{
	if (Nickname.IsEmpty())
	{
		return false;
	}

	// 중복 확인 + 등록을 원자적으로 처리
	if (PlayerInfoMap.Contains(Nickname))
	{
		return false; // 이미 사용 중
	}

	// 캐릭터는 나중에 등록되므로 기본값 설정
	FPlayerInfo Info;
	Info.Nickname = Nickname;
	Info.CharacterType = ECharacterType::Knight; // 기본값 또는 None 등

	PlayerInfoMap.Add(Nickname, Info);
	return true;
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
	return ECharacterType::Knight;
}

void UGI_BattleInstance::AddPlayerSpawnInfo(int32 PlayerNum, TSubclassOf<APawn> CharacterClass)
{
	FPlayerInfo NewInfo;
	NewInfo.PlayerNum = PlayerNum;
	NewInfo.CharacterClass = CharacterClass;
	CachedSpawnList.Add(NewInfo);
}
