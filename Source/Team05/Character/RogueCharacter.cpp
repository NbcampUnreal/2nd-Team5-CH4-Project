//RougeCharacter.h

#include "RogueCharacter.h"

#include "EnhancedInputComponent.h"
#include "MyPlayerController.h"


// Sets default values
ARogueCharacter::ARogueCharacter()
{
	
}

void ARogueCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void ARogueCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
					&ARogueCharacter::SpecialAttack_Input
				);
			}
		}
	}
}

void ARogueCharacter::SpecialAttack_Input()
{
	if (bInputEnabled && bCanSpecialAttack)
	{
		SpecialAttack();
		SetCooldownTimer(SpecialAttackCooldown);
	}
}

void ARogueCharacter::SpecialAttack()
{
	ServerRPCAttack(SpecialAttackAnimMontage);
	// Fire();
	PlayAnimMontage(SpecialAttackAnimMontage);
}

void ARogueCharacter::SpecialUpperAttack()
{
}

void ARogueCharacter::SpecialLowerAttack()
{
}

void ARogueCharacter::SpecialFrontAttack()
{
}

void ARogueCharacter::Fire_Implementation()
{
	if (ProjectileClass)
	{
		MuzzleOffset.Set(70.0f, 0.0f, 0.0f);
		const FVector MuzzleLocation = GetActorLocation() + GetActorForwardVector() * MuzzleOffset;
		
		const FRotator MuzzleRotation = GetActorRotation();

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
                    
			// 총구에 발사체를 스폰합니다.
			ARogueProjectile* Projectile = World->SpawnActor<ARogueProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// 발사체의 초기 탄도를 설정합니다.
				const FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
}

