// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_BattleInstance.h"

bool UGI_BattleInstance::RegisterPlayerID(const FString& ID, FString& OutNickname, int32& OutPlayerNum)
{
	// 새로운 플레이어 번호 부여
	int32 NewPlayerNum = PlayerInfoMap.Num() + 1;

	FPlayerInfo NewInfo;
	NewInfo.ID = ID;
	NewInfo.PlayerNum = NewPlayerNum;
	NewInfo.Nickname = TEXT(""); // 닉네임은 나중에 등록
	NewInfo.CharacterClass = nullptr;

	PlayerInfoMap.Add(ID, NewInfo);

	OutNickname = NewInfo.Nickname;
	OutPlayerNum = NewPlayerNum;

	return true;
}

bool UGI_BattleInstance::RegisterNickname(const FString& ID, const FString& Nickname)
{
	// 닉네임 중복 체크
	for (const auto& Pair : PlayerInfoMap)
	{
		if (Pair.Value.Nickname == Nickname)
		{
			return false;
		}
	}

	// ID 기반으로 닉네임 등록
	if (FPlayerInfo* Info = PlayerInfoMap.Find(ID))
	{
		Info->Nickname = Nickname;
		return true;
	}

	return false;
}

bool UGI_BattleInstance::RegisterCharacterClass(const FString& ID, TSubclassOf<APawn> CharacterClass)
{
	if (FPlayerInfo* Info = PlayerInfoMap.Find(ID))
	{
		Info->CharacterClass = CharacterClass;
		return true;
	}
	return false;
}

TSubclassOf<APawn> UGI_BattleInstance::GetCharacterClass(const FString& ID) const
{
	if (const FPlayerInfo* Info = PlayerInfoMap.Find(ID))
	{
		return Info->CharacterClass;
	}

	return nullptr;
}

void UGI_BattleInstance::AddPlayerSpawnInfo(int32 PlayerNum, TSubclassOf<APawn> CharacterClass)
{
	FPlayerInfo NewInfo;
	NewInfo.PlayerNum = PlayerNum;
	NewInfo.CharacterClass = CharacterClass;

	CachedSpawnList.Add(NewInfo);
}

