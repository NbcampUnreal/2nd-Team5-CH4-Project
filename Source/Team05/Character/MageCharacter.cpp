//MageCharacter.cpp


#include "MageCharacter.h"

#include "EnhancedInputComponent.h"
#include "MyPlayerController.h"


AMageCharacter::AMageCharacter()
{
	
}

void AMageCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMageCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
					&AMageCharacter::SpecialAttack_Input
				);
			}
		}
	}
}

void AMageCharacter::SpecialAttack_Input()
{
	if (bInputEnabled)
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
	}
}

void AMageCharacter::SpecialAttack()
{
	Fire();
}

void AMageCharacter::SpecialUpperAttack()
{
}

void AMageCharacter::SpecialLowerAttack()
{
}

void AMageCharacter::SpecialFrontAttack()
{
}

void AMageCharacter::Fire_Implementation()
{
	if (ProjectileClass)
	{
		MuzzleOffset.Set(10.0f, 0.0f, 0.0f);
		const FVector MuzzleLocation = GetActorLocation() + GetActorForwardVector() * MuzzleOffset;
		
		const FRotator MuzzleRotation = GetActorRotation();

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
                    
			// 총구에 발사체를 스폰합니다.
			AMageProjectile* Projectile = World->SpawnActor<AMageProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// 발사체의 초기 탄도를 설정합니다.
				const FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
	
}

