// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Team05/Ability/AbilityComponentKnight.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 코드로 직접 로테이션 제어하기 위해 비활성화
	bUseControllerRotationYaw = false;
	// 공중에서 좌우 컨트롤 배율 100퍼센트로 지정
	GetCharacterMovement()->AirControl = 1.0f;
	FatigueRate = 0;
	Life = 3;

	AbilityComponent = CreateDefaultSubobject<UAbilityComponentKnight>("Ability Component");
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(GetController()))
		{
			// 캐릭터 이동 바인딩
			if (MyPlayerController->MoveAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ABaseCharacter::Move1D
				);
			}
	
			// 캐릭터 점프 바인딩
			if (MyPlayerController->JumpAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ABaseCharacter::JumpStart
				);
	
				EnhancedInputComponent->BindAction(
					MyPlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ABaseCharacter::JumpEnd
				);
			}

			// 캐릭터 키입력 방향 바인딩
			if (MyPlayerController->DirectionAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->DirectionAction,
					ETriggerEvent::Triggered,
					this,
					&ABaseCharacter::SetDirection
				);
			}
	
			// 캐릭터 기본 공격 바인딩
			if (MyPlayerController->BaseAttackAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->BaseAttackAction,
					ETriggerEvent::Triggered,
					this,
					&ABaseCharacter::BaseAttack
				);
			}
	
			// 캐릭터 특수 공격 바인딩
			if (MyPlayerController->SpecialAttackAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->SpecialAttackAction,
					ETriggerEvent::Triggered,
					this,
					&ABaseCharacter::SpecialAttack
				);
			}
	
			// 캐릭터 특수 이동 바인딩
			if (MyPlayerController->SpecialMoveAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->SpecialMoveAction,
					ETriggerEvent::Triggered,
					this,
					&ABaseCharacter::SpecialMove
				);
			}
	
			// 캐릭터 가드 바인딩
			if (MyPlayerController->GuardAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->GuardAction,
					ETriggerEvent::Triggered,
					this,
					&ABaseCharacter::Guard
				);
			}
	
			// 캐릭터 이모지 바인딩
			if (MyPlayerController->EmoteAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->EmoteAction,
					ETriggerEvent::Triggered,
					this,
					&ABaseCharacter::Emote
				);
			}
		}
	}
}

void ABaseCharacter::Move1D(const FInputActionValue& Value)
{
	if (!Controller) return;

	if (const float MoveInput = Value.Get<float>(); !FMath::IsNearlyZero(MoveInput))
	{
		AddMovementInput(MoveInput * GetActorRightVector(), MoveInput);
		// 캐릭터 회전 반영
		const FRotator NewRotation = FRotator(0.f, MoveInput > 0.f ? 0.f : 180.f, 0.f);
		SetActorRotation(NewRotation);
	}
}

void ABaseCharacter::JumpStart(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void ABaseCharacter::JumpEnd(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void ABaseCharacter::SetDirection(const FInputActionValue& Value)
{
	const FVector2D InputDirection = Value.Get<FVector2D>();

	// 키보드 입력을 구분하기 위한 각도 계산
	// 위   90   (x = 0, y = 1)
	// 아래 -90  (x = 0, y = 0)
	// 앞   0    (x = 1, u = 0)
	const float AngleDegrees = FMath::RadiansToDegrees(FMath::Atan2(InputDirection.X, InputDirection.Y));
	
	if (AngleDegrees > 45.0f && AngleDegrees < 135.0f)
	{
		CurrentDirection = EDirectionEnum::EUp;
	}
	else if (AngleDegrees < -45.0f && AngleDegrees >= -135.0f) 
	{
		CurrentDirection = EDirectionEnum::EDown;
	}
	else if (AngleDegrees > 45.0f && AngleDegrees <= 45.0f)
	{
		CurrentDirection = EDirectionEnum::EForward;
	}
	else
	{
		CurrentDirection = EDirectionEnum::ENone;
	}
}

void ABaseCharacter::BaseAttack(const FInputActionValue& Value)
{
}

void ABaseCharacter::SpecialAttack(const FInputActionValue& Value)
{
	AbilityComponent->SpecialAttack();
}

void ABaseCharacter::SpecialMove(const FInputActionValue& Value)
{
	
}

void ABaseCharacter::Guard(const FInputActionValue& Value)
{
}

void ABaseCharacter::Emote(const FInputActionValue& Value)
{
}
