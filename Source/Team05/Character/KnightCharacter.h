// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Ability/SpecialAttack.h"
#include "UObject/Object.h"
#include "KnightCharacter.generated.h"

UCLASS()
class TEAM05_API AKnightCharacter : public ABaseCharacter, public ISpecialAttack
{
	GENERATED_BODY()

public:
	AKnightCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SpecialAttack_Input();
	virtual void SpecialAttack() override;
	virtual void SpecialUpperAttack() override;
	virtual void SpecialLowerAttack() override;
	virtual void SpecialFrontAttack() override;

	void SetCooldownTimer(float Cooldown);
	
protected:
	bool bCanSpecialAttack;

	FTimerHandle CooldownTimer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialUpperAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialLowerAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialFrontAttackAnimMontage;
};
