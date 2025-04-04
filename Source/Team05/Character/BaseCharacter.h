// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UInputAction;
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
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 피로도 (높을 수록 멀리 타격됨, 매치마다 0으로 초기화)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 FatigueRate;
	// 생명 (매치마다 3개로 초기화)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Life;
	// 현재 키보드 입력 방향
	EDirectionEnum CurrentDirection;

	float BaseAttackMontagePlayTime;
	float LastStartAttackTime;
	float AttackTimeDifference;

	// 몽타주 리스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> BaseAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> GuardMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> EmoteMontage;

	// 서버
	UPROPERTY(ReplicatedUsing = OnRep_ServerRotationYaw)
	float ServerRotationYaw;
	UPROPERTY(ReplicatedUsing = OnRep_InputEnabled)
	uint8 bInputEnabled : 1;

	UFUNCTION()
	void OnRep_ServerRotationYaw();
	UFUNCTION()
	void OnRep_TakeDamage();
	UFUNCTION()
	void OnRep_InputEnabled();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCAttack();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttack(float InStartAttackTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	void CheckAttackHit();
	
	UFUNCTION()
	void Move1D(const FInputActionValue& Value);
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

	void PlayMontage(const TObjectPtr<UAnimMontage>& Montage);
};
