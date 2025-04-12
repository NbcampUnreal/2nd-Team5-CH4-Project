// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PS_PlayerState.generated.h"


UCLASS()
class TEAM05_API APS_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	APS_PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 플레이어 번호
	UPROPERTY(Replicated)
	int32 PlayerNum;

	// 닉네임
	UPROPERTY(Replicated)
	FString Nickname;

	// 선택된 캐릭터 클래스
	UPROPERTY(Replicated)
	TSubclassOf<APawn> CharacterClass;

	// 준비 여부
	UPROPERTY(Replicated)
	bool bReady;

	// Getter / Setter
	void SetReady(bool bInReady);
	bool IsReady() const;

	// Getter / Setter
	void SetPlayerNum(int32 InNum);
	int32 GetPlayerNum() const;

	void SetPlayerNickName(FString InNickname);
	FString GetPlayerNickName() const;

	void SetCharacterClass(TSubclassOf<APawn> InClass);
	TSubclassOf<APawn> GetCharacterClass() const;
};
