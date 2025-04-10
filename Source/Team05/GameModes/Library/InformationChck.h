// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameModes/GI_BattleInstance.h"
#include "InformationChck.generated.h"

/**
 * 
 */
UCLASS()
class TEAM05_API UInformationChck : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	// 닉네임이 사용 가능한지 서버에서 확인
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "PlayerInfo")
	void Server_CheckNickname(UObject* WorldContextObject, const FString& Nickname);

	// 닉네임 등록 요청
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "PlayerInfo")
	void Server_RegisterNickname(UObject* WorldContextObject, const FString& Nickname);

	// 캐릭터 타입 등록 요청
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "PlayerInfo")
	void Server_RegisterCharacterType(UObject* WorldContextObject, const FString& Nickname, ECharacterType CharacterType);

	// 캐릭터 타입 조회는 클라에서도 가능 (읽기 전용이라면)
	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	ECharacterType GetCharacterTypeByNickname(UObject* WorldContextObject, const FString& Nickname);

};
