#include "AbilityComponentBarbarian.h"
#include "Team05/Character/BaseCharacter.h"

UAbilityComponentBarbarian::UAbilityComponentBarbarian()
{
	PrimaryComponentTick.bCanEverTick = false;

	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());

	bCanAttack = true;
}

void UAbilityComponentBarbarian::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAbilityComponentBarbarian::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAbilityComponentBarbarian::SpecialAttack()
{
	if (bCanAttack == false)
	{
		return;
	}
	
	if (IsValid(OwnerCharacter) == true)
	{
		bCanAttack = false;
		float MontagePlayTime = SpecialAttackAnimMontage->GetPlayLength();

		ServerRPCAttack(SpecialAttackAnimMontage, 500.f, 0.f);
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]() -> void
		{
			bCanAttack = true;
		}), MontagePlayTime, false, -1.f);
	}
}

void UAbilityComponentBarbarian::SpecialUpperAttack()
{
	if (bCanAttack == false)
	{
		return;
	}

	if (IsValid(OwnerCharacter) == true)
	{
		bCanAttack = false;
		float MontagePlayTime = SpecialUpperAttackAnimMontage->GetPlayLength();

		ServerRPCAttack(SpecialUpperAttackAnimMontage, 0.f, 700.f);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]() -> void
		{
			bCanAttack = true;
		}), MontagePlayTime, false, -1.f);
	}
}

void UAbilityComponentBarbarian::SpecialLowerAttack()
{
	
}

void UAbilityComponentBarbarian::SpecialFrontAttack()
{
	if (bCanAttack == false)
	{
		return;
	}

	if (IsValid(OwnerCharacter) == true)
	{
		bCanAttack = false;
		float MontagePlayTime = SpecialFrontAttackAnimMontage->GetPlayLength();

		ServerRPCAttack(SpecialFrontAttackAnimMontage, 1000.f, 0.f);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]() -> void
		{
			bCanAttack = true;
		}), MontagePlayTime, false, -1.f);
	}
}

void UAbilityComponentBarbarian::ServerRPCAttack_Implementation(UAnimMontage* Montage, float LaunchXDistance,
	float LaunchZDistance)
{
	LaunchCharacter(LaunchXDistance, LaunchZDistance);
	MulticastRPCAttack(Montage, LaunchXDistance, LaunchZDistance);
}

void UAbilityComponentBarbarian::MulticastRPCAttack_Implementation(UAnimMontage* Montage, float LaunchXDistance,
                                                                float LaunchZDistance)
{
	if (OwnerCharacter->GetMesh()->GetAnimInstance()->GetCurrentActiveMontage() == Montage)
	{
		return;
	}
	
	OwnerCharacter->PlayMontage(Montage);
}

void UAbilityComponentBarbarian::LaunchCharacter(float LaunchXDistance, float LaunchZDistance)
{
	if (IsValid(OwnerCharacter) == false)
	{
		return;
	}

	float XDirection = OwnerCharacter->GetActorForwardVector().X;
	XDirection = (XDirection >= 0.f) ? 1.f : -1.f;
	const FVector LaunchVelocity = FVector(XDirection * LaunchXDistance, 0.f, 200.f + LaunchZDistance);
	OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);
}

