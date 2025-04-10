// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/GI_BattleInstance.h"
#include "PS_LobbyPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAM05_API APS_LobbyPlayer : public APlayerState
{
	GENERATED_BODY()
	
public:
	APS_LobbyPlayer();

	// 닉네임
	void SetNickname(const FString& NewNickname);
	FString GetNickname() const;

	// 캐릭터 타입
	void SetCharacterType(ECharacterType NewType);
	ECharacterType GetCharacterType() const;

	// Ready 상태
	void SetReady(bool bNewReady);
	bool IsReady() const;

protected:
	// 복제 등록
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// 닉네임 (서버 → 클라이언트 동기화)
	UPROPERTY(Replicated)
	FString Nickname;

	// 캐릭터 타입 (서버 → 클라이언트 동기화)
	UPROPERTY(Replicated)
	ECharacterType CharacterType;

	// 준비 여부
	UPROPERTY(Replicated)
	bool bReady;
};
