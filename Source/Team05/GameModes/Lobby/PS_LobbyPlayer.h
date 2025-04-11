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

	void SetNickname(const FString& NewNickname);
	FString GetNickname() const;

	void SetCharacterType(ECharacterType NewType);
	ECharacterType GetCharacterType() const;
	void RegisterToGameInstance();

	void SetReady(bool bNewReady);
	bool IsReady() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	FString Nickname;

	UPROPERTY(Replicated)
	ECharacterType CharacterType;

	UPROPERTY(Replicated)
	bool bReady;
};
