// Fill out your copyright notice in the Description page of Project Settings.


#include "BarbarianCharacter.h"

#include "EnhancedInputComponent.h"
#include "MyPlayerController.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "Kismet/GameplayStatics.h"

ABarbarianCharacter::ABarbarianCharacter()
{
	bCanSpecialAttack = true;
}

void ABarbarianCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABarbarianCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(CooldownTimer);
	CooldownTimer.Invalidate();
}

void ABarbarianCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(GetController()))
		{
			// 캐릭터 특수 공격 바인딩
			if (MyPlayerController->SpecialAttackAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->SpecialAttackAction,
					ETriggerEvent::Started,
					this,
					&ABarbarianCharacter::SpecialAttack_Input
				);
			}
		}
	}
}

void ABarbarianCharacter::SpecialAttack_Input()
{
	if (bInputEnabled && bCanSpecialAttack)
	{
		if (CurrentDirection == EDirectionEnum::EUp)
		{
			SpecialUpperAttack();
		}
		else if (CurrentDirection == EDirectionEnum::EForward)
		{
			SpecialFrontAttack();
		}
		else if (CurrentDirection == EDirectionEnum::EDown)
		{
			SpecialLowerAttack();
		}
		else
		{
			SpecialAttack();
		}
		
		SetCooldownTimer(5.f);
	}
}

void ABarbarianCharacter::SpecialAttack()
{
	ServerRPCAttack(SpecialAttackAnimMontage);
	float ChargingTime = SpecialAttackAnimMontage->GetSectionLength(0);
	
	PlayMontage(SpecialAttackAnimMontage);
	
	FTimerHandle AttackChargeTimerHandle;
	if (IsValid(GetWorld()))
	{
		GetWorldTimerManager().SetTimer(AttackChargeTimerHandle,FTimerDelegate::CreateLambda([&]()
		{
			Launch(500.f, 0.f);
		}), ChargingTime, false, -1.f);
	}
}

void ABarbarianCharacter::SpecialUpperAttack()
{
	ServerRPCAttack(SpecialUpperAttackAnimMontage);
	float ChargingTime = SpecialUpperAttackAnimMontage->GetSectionLength(0);
	
	PlayMontage(SpecialUpperAttackAnimMontage);
	
	FTimerHandle UpperChargeTimerHandle;
	if (IsValid(GetWorld()))
	{
		GetWorldTimerManager().SetTimer(UpperChargeTimerHandle,FTimerDelegate::CreateLambda([&]()
		{
			Launch(0.f, 700.f);
		}), ChargingTime, false, -1.f);
	}
}

void ABarbarianCharacter::SpecialLowerAttack()
{
	ServerRPCAttack(SpecialLowerAttackAnimMontage);
	float ChargingTime = SpecialLowerAttackAnimMontage->GetSectionLength(0);
	
	PlayMontage(SpecialLowerAttackAnimMontage);
	
	FTimerHandle LowerChargeTimerHandle;
	if (IsValid(GetWorld()))
	{
		GetWorldTimerManager().SetTimer(LowerChargeTimerHandle,FTimerDelegate::CreateLambda([&]()
		{
			Launch(0.f, -1000.f);
		}), ChargingTime, false, -1.f);
	}
}

void ABarbarianCharacter::SpecialFrontAttack()
{
	ServerRPCAttack(SpecialFrontAttackAnimMontage);
	
	PlayMontage(SpecialFrontAttackAnimMontage);
	Launch(900.f, 0.f);
}

void ABarbarianCharacter::SetCooldownTimer(float Cooldown)
{
	bCanSpecialAttack = false;
	if (IsValid(GetWorld()) == true)
	{
		GetWorldTimerManager().SetTimer(CooldownTimer, FTimerDelegate::CreateLambda([&]()
		{
			bCanSpecialAttack = true;
		}), Cooldown, false);
	}
}
