#include "Team05AnimInstance.h"

#include "Elements/Framework/TypedElementQueryBuilder.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Team05/Character/BaseCharacter.h"

void UTeam05AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* PossessedPawn = TryGetPawnOwner();
	if (IsValid(PossessedPawn) == true)
	{
		if (PossessedPawn->IsPlayerControlled() == false)
		{
			bIsAIControlled = true;
		}
	}
	
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

	bIsFalling = OwnerCharacterMovementComponent->IsFalling();
	Velocity = OwnerCharacterMovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	
	if (bIsAIControlled == true)
	{
		bShouldMove = GroundSpeed > 3.f;
	}
	else
	{

		bShouldMove = ((OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) == false) && (GroundSpeed > 3.f);
	}
}
