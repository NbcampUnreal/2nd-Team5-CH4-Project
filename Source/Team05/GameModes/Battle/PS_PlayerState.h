// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PS_PlayerState.generated.h"

UENUM(BlueprintType)
enum class ECharacterType2 : uint8
{
	Knight UMETA(DisplayName = "Knight"),
	Knight_2 UMETA(DisplayName = "Knight")

};


UCLASS()
class TEAM05_API APS_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	APS_PlayerState();

	void SetCharacterType(ECharacterType2 InType);
	ECharacterType2 GetCharacterType() const;

protected:
	UPROPERTY(Replicated)
	ECharacterType2 CharacterType;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
