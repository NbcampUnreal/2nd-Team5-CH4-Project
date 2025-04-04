// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UAbilityComponentKnight;
struct FInputActionValue;

UENUM()
enum EDirectionEnum
{
	ENone,
	EForward,
	EUp,
	EDown,
};

UCLASS()
class TEAM05_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 피로도 (높을 수록 멀리 타격됨, 매치마다 0으로 초기화)
	UPROPERTY()
	int32 FatigueRate;
	// 생명 (매치마다 3개로 초기화)
	UPROPERTY()
	int32 Life;
	// 현재 키보드 입력 방향
	EDirectionEnum CurrentDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAbilityComponentKnight> AbilityComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move1D(const FInputActionValue& Value);
	UFUNCTION()
	void JumpStart(const FInputActionValue& Value);
	UFUNCTION()
	void JumpEnd(const FInputActionValue& Value);
	UFUNCTION()
	void SetDirection(const FInputActionValue& Value);
	UFUNCTION()
	void BaseAttack(const FInputActionValue& Value);
	UFUNCTION()
	virtual void SpecialAttack(const FInputActionValue& Value);
	UFUNCTION()
	virtual void SpecialMove(const FInputActionValue& Value);
	UFUNCTION()
	void Guard(const FInputActionValue& Value);
	UFUNCTION()
	void Emote(const FInputActionValue& Value);
};
