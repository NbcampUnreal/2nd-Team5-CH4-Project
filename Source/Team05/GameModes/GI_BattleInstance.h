// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GI_BattleInstance.generated.h"

// 캐릭터 선택 정보 구조체
USTRUCT(BlueprintType)
struct FPlayerSelectInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Nickname;

	UPROPERTY(BlueprintReadWrite)
	FName CharacterType;
};

/**
 * 
 */
UCLASS()
class TEAM05_API UGI_BattleInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Player Info")
	bool HasCompletedLobbySetup() const;

	UFUNCTION(BlueprintCallable, Category = "Player Info")
	void SetCompletedLobbySetup(bool bCompleted);

	UFUNCTION(BlueprintCallable, Category = "Player Info")
	const FPlayerSelectInfo& GetSelectInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Player Info")
	void SetSelectInfo(const FPlayerSelectInfo& Info);

	UFUNCTION(BlueprintCallable)
	void ResetSelection();

private:
	bool bHasCompletedLobbySetup = false;
	FPlayerSelectInfo SelectInfo;
};
