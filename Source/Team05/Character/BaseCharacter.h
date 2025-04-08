// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UInputAction;
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

UENUM()
enum ECharacterState
{
    STATE_Idle,
	STATE_Attack,
	STATE_Guard,
	STATE_OnAttacked
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
	TObjectPtr<UAnimMontage> GuardMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> EmoteMontage;

	// 서버

	// 플레이어 입력 가능 여부
	// 공격, 특수 공격, 가드 중 다른 입력 막기 (bInputEnabled = true)
	// 각 애니메이션이 끝났을 때 replication 됨
	UPROPERTY(ReplicatedUsing = OnRep_InputEnabled)
	uint8 bInputEnabled : 1;
	
	ECharacterState CurrentState;

#pragma region Guard
	UPROPERTY(ReplicatedUsing = OnRep_GuardState)
	uint8 bOnGuard : 1;
	int32 GuardStamina;
	int32 MaxGuardStamina;
	int32 GuardDamageReduction;
	FTimerHandle GuardStaminaTimer;

	UFUNCTION()
	void OnRep_GuardState();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCStartGuard();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCStopGuard();
	
#pragma endregion
	
	UFUNCTION()
	void OnRep_TakeDamage();
	UFUNCTION()
	void OnRep_InputEnabled();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCAttack();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttack(float InStartAttackTime);

	UFUNCTION(Server, Reliable)
	void ServerRotateCharacter(float YawValue);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAbilityComponentKnight> AbilityComponent;

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
	void StartGuard(const FInputActionValue& Value);
	UFUNCTION()
	void StopGuard();
	UFUNCTION()
	void Emote(const FInputActionValue& Value);

	void PlayMontage(const TObjectPtr<UAnimMontage>& Montage);

	void DrawDebugMeleeAttack(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward);
};
