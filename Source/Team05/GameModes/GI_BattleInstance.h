// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GI_BattleInstance.generated.h"

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	Knight	UMETA(DisplayName = "Knight"),
	Mage	UMETA(DisplayName = "Mage")
};

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString Nickname;

	UPROPERTY()
	ECharacterType CharacterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> CharacterClass;

	FPlayerInfo()
		: Nickname(TEXT("")), CharacterType(ECharacterType::Knight)
	{
	}

	FPlayerInfo(const FString& InNickname, ECharacterType InType)
		: Nickname(InNickname), CharacterType(InType)
	{
	}
};


UCLASS()
class TEAM05_API UGI_BattleInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	// 중복 확인과 동시에 등록 (Atomic)
	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	bool TryRegisterNickname(const FString& Nickname);

	// 캐릭터 선택 시 정보 업데이트
	void UpdateCharacterType(const FString& Nickname, ECharacterType NewType);

	// 선택된 캐릭터 확인
	ECharacterType GetCharacterTypeByNickname(const FString& Nickname) const;

public:
	UPROPERTY(EditAnywhere, Category = "Character")
	TMap<ECharacterType, TSubclassOf<APawn>> CharacterClassMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TArray<FPlayerInfo> CachedSpawnList;

	UFUNCTION(BlueprintCallable)
	void AddPlayerSpawnInfo(int32 PlayerNum, TSubclassOf<APawn> CharacterClass);

private:

	UPROPERTY()
	TMap<FString, FPlayerInfo> PlayerInfoMap;

	
};
