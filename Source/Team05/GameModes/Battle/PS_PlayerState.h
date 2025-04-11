// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/GI_BattleInstance.h"
#include "PS_PlayerState.generated.h"

<<<<<<< HEAD
/**
 * 
 */
=======
UENUM(BlueprintType)
enum class ECharacterType2 : uint8
{
	Knight UMETA(DisplayName = "Knight"),
	Knight_2 UMETA(DisplayName = "Knight")

};


>>>>>>> feature/Spawn
UCLASS()
class TEAM05_API APS_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	APS_PlayerState();

	void SetNickname(const FString& NewNickname);
	FString GetNickname() const;

	void SetCharacterType(ECharacterType NewType);
	ECharacterType GetCharacterType() const;
	void RegisterToGameInstance();

	void SetReady(bool bNewReady);
	bool IsReady() const;

protected:
<<<<<<< HEAD
=======
	UPROPERTY(Replicated)
	ECharacterType2 CharacterType;

>>>>>>> feature/Spawn
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	FString Nickname;

	UPROPERTY(Replicated)
	ECharacterType CharacterType;

	UPROPERTY(Replicated)
	bool bReady;
};
