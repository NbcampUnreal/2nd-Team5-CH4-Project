#include "AbilityComponentKnight.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Components/CapsuleComponent.h"
#include "Team05/Character/BaseCharacter.h"

UAbilityComponentKnight::UAbilityComponentKnight()
{
	PrimaryComponentTick.bCanEverTick = false;

	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());

	bCanAttack = true;
}

void UAbilityComponentKnight::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAbilityComponentKnight::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAbilityComponentKnight::SpecialAttack()
{
	if (bCanAttack == false)
	{
		return;
	}
	
	if (IsValid(OwnerCharacter) == true)
	{
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance) == true)
		{
			bCanAttack = false;
			float MontagePlayTime = SpecialAttackAnimMontage->GetPlayLength();
			AnimInstance->StopAllMontages(true);
			AnimInstance->Montage_Play(SpecialAttackAnimMontage);
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]() -> void
		{
			OnAttackMontageEnded();
		}), MontagePlayTime, false, -1.f);
		}
	}

	TArray<FHitResult> HitResults;
	TSet<ACharacter*> DamagedCharacters;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	const float AttackDamage = 10.f;
	const float AttackRange = 150.f;
	const float AttackRadius = 100.f;
	const FVector Forward = GetOwner()->GetActorForwardVector();
	const FVector Start = GetOwner()->GetActorLocation() + Forward * OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() * 2.f;
	const FVector End = Start + Forward * AttackRange;

	bool bIsHitDetected = GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECC_Camera, FCollisionShape::MakeSphere(AttackRadius), QueryParams);
	if (bIsHitDetected == true)
	{
		for (auto const& HitResult : HitResults)
		{
			ABaseCharacter* DamagedCharacter = Cast<ABaseCharacter>(HitResult.GetActor());
			if (IsValid(DamagedCharacter) == true)
			{
				DamagedCharacters.Add(DamagedCharacter);
			}
		}

		FDamageEvent DamageEvent;
		for (auto const& DamagedCharacter : DamagedCharacters)
		{
			UGameplayStatics::ApplyDamage(DamagedCharacter, AttackDamage, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());
		}
	}

	FColor DrawColor = bIsHitDetected ? FColor::Green : FColor::Red;
	DrawDebugAttack(DrawColor, Start, End, Forward);
}

void UAbilityComponentKnight::SpecialUpperAttack()
{
	
}

void UAbilityComponentKnight::SpecialLowerAttack()
{
	
}

void UAbilityComponentKnight::SpecialFrontAttack()
{
	
}

void UAbilityComponentKnight::CheckAttackHit()
{
}

void UAbilityComponentKnight::OnAttackMontageEnded()
{
	bCanAttack = true;
}

void UAbilityComponentKnight::DrawDebugAttack(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd,
	FVector Forward)
{
	const float MeleeAttackRange = 150.f;
	const float MeleeAttackRadius = 100.f;
	FVector CapsuleOrigin = TraceStart + (TraceEnd - TraceStart) * 0.5f;
	float CapsuleHalfHeight = MeleeAttackRange * 0.5f;
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, MeleeAttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 5.0f);
}

