// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GI_BattleInstance.generated.h"

// 캐릭터 종류를 나타내는 열거형
UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	//일단 나이트 추후 추가하면 됨
	Knight UMETA(DisplayName = "Knight"),
	Knight_2 UMETA(DisplayName = "Knight")
};

// 한 명의 플레이어 정보를 저장할 구조체 (닉네임 + 캐릭터 종류)
USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString Nickname;

	UPROPERTY()
	ECharacterType CharacterType;

	FPlayerInfo()
		: Nickname(TEXT("")), CharacterType(ECharacterType::Knight)
	{
	}

	FPlayerInfo(const FString& InNickname, ECharacterType InType)
		: Nickname(InNickname), CharacterType(InType)
	{
	}
};

/**
 * 
 */
UCLASS()
class TEAM05_API UGI_BattleInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// 닉네임 중복 확인
	bool IsNicknameTaken(const FString& Nickname) const;

	// 플레이어 정보 등록 (닉네임 + 캐릭터)
	void RegisterPlayerInfo(const FString& Nickname, ECharacterType Type);

	// GI_BattleInstance.h
	void UpdateCharacterType(const FString& Nickname, ECharacterType NewType);

	// 닉네임으로 캐릭터 종류 조회
	ECharacterType GetCharacterTypeByNickname(const FString& Nickname) const;

private:
	// 닉네임을 키로 한 플레이어 정보 맵
	UPROPERTY()
	TMap<FString, FPlayerInfo> PlayerInfoMap;
};
