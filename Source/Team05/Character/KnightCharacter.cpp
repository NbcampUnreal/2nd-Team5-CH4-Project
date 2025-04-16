//KnightCharacter.cpp


#include "KnightCharacter.h"

#include "EnhancedInputComponent.h"
#include "MyPlayerController.h"

AKnightCharacter::AKnightCharacter()
{
}

void AKnightCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AKnightCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
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
					&AKnightCharacter::SpecialAttack_Input
				);
			}
		}
	}
}
	
void AKnightCharacter::SpecialAttack_Input()
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
		
		SetCooldownTimer(SpecialAttackCooldown);
	}
}

void AKnightCharacter::SpecialAttack()
{
	ServerRPCAttack(SpecialAttackAnimMontage);
	Launch(500.f, 0.f);
	PlayMontage(SpecialAttackAnimMontage);
}

void AKnightCharacter::SpecialUpperAttack()
{
	ServerRPCAttack(SpecialUpperAttackAnimMontage);
	Launch(0.f, 700.f);
	PlayMontage(SpecialUpperAttackAnimMontage);
}

void AKnightCharacter::SpecialLowerAttack()
{
	ServerRPCAttack(SpecialLowerAttackAnimMontage);
	Launch(0.f, -1000.f);
	PlayMontage(SpecialLowerAttackAnimMontage);
}

void AKnightCharacter::SpecialFrontAttack()
{
	ServerRPCAttack(SpecialFrontAttackAnimMontage);
	Launch(1000.f, 0.f);
	PlayMontage(SpecialFrontAttackAnimMontage);
}
