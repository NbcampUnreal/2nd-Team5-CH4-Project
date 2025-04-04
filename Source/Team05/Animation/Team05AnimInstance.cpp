#include "Team05AnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Team05/Character/BaseCharacter.h"

void UTeam05AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ACharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UTeam05AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(OwnerCharacter) == false || IsValid(OwnerCharacterMovementComponent) == false)
	{
		return;
	}

	Velocity = OwnerCharacterMovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	bShouldMove = ((OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) == false) && (GroundSpeed > 3.f);
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();
}

void UTeam05AnimInstance::AnimNotify_CheckAttackHit()
{
	ABaseCharacter* OwnerPlayerCharacter = Cast<ABaseCharacter>(OwnerCharacter);
	if (IsValid(OwnerPlayerCharacter))
	{
		OwnerPlayerCharacter->CheckAttackHit();
	}
}

