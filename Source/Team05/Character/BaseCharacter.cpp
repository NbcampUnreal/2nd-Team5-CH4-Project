// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
	: 
	FatigueRate(0),
	Life(3),
	LastStartAttackTime(0.f),
	AttackTimeDifference(0.f),
	bInputEnabled(true)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 코드로 직접 로테이션 제어하기 위해 비활성화
	bUseControllerRotationYaw = false;
	// 공중에서 좌우 컨트롤 배율 100퍼센트로 지정
	GetCharacterMovement()->AirControl = 1.0f;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, ServerRotationYaw);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(BaseAttackMontage))
	{
		BaseAttackMontagePlayTime = BaseAttackMontage->GetPlayLength();	
	}
}

void ABaseCharacter::OnRep_ServerRotationYaw()
{
	SetActorRotation(FRotator(0.f, ServerRotationYaw, 0.f));
}

void ABaseCharacter::OnRep_TakeDamage()
{
	// 피격 애니메이션 실행

	// 피격 이펙트 실행

	//피격 사운드 실행

	GEngine->AddOnScreenDebugMessage(
		-1,
		5.0f,                  
		FColor::Blue,
		FString::Printf(TEXT("피격됨 피로도: %d"), FatigueRate)
	);
}

void ABaseCharacter::OnRep_InputEnabled()
{
	if (bInputEnabled)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	}
}

void ABaseCharacter::MulticastRPCAttack_Implementation()
{
	if (HasAuthority() == false && IsLocallyControlled() == false)
	{
		PlayMontage(BaseAttackMontage);
	}
}

void ABaseCharacter::ServerAttack_Implementation(float InStartAttackTime)
{
	AttackTimeDifference = GetWorld()->GetTimeSeconds() - InStartAttackTime;
	AttackTimeDifference = FMath::Clamp(AttackTimeDifference, 0.f, BaseAttackMontagePlayTime);

	if (FMath::IsNearlyEqual(AttackTimeDifference ,AttackTimeDifference))
	{
		bInputEnabled = false;
		// OnRep_InputEnabled();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			// bInputEnabled = true;
			// OnRep_InputEnabled();
		}), BaseAttackMontagePlayTime - AttackTimeDifference, false, -1.f);
	}

	LastStartAttackTime = InStartAttackTime;
	PlayMontage(BaseAttackMontage);

	MulticastRPCAttack();
}

bool ABaseCharacter::ServerAttack_Validate(float InStartAttackTime)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("PIE Client Index: %d"), static_cast<int32>(GPlayInEditorID));
#endif
	FName Name = *GetName();
	if (LastStartAttackTime == 0.0f)
	{
		// 최초 공격은 일단 통과.
		return true;
	}

	return (BaseAttackMontagePlayTime - 0.1f) < (InStartAttackTime - LastStartAttackTime);
}

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority())
	{
		return 0;
	}
	
	FatigueRate += DamageAmount;
	return DamageAmount;
}


void ABaseCharacter::CheckAttackHit()
{
	if (IsLocallyControlled())
	{
		TArray<FHitResult> OutHitResults;
		TSet<ACharacter*> DamagedCharacters;
		FCollisionQueryParams Params(NAME_None, false, this);

		const float MeleeAttackRange = 50.f;
		const float MeleeAttackRadius = 50.f;
		//const float MeleeAttackDamage = 10.f;
		const FVector Forward = GetActorForwardVector();
		const FVector Start = GetActorLocation() + Forward * GetCapsuleComponent()->GetScaledCapsuleRadius();
		const FVector End = Start + GetActorForwardVector() * MeleeAttackRange;

		bool bIsHitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, Start, End, FQuat::Identity, ECC_Camera, FCollisionShape::MakeSphere(MeleeAttackRadius), Params);
		if (bIsHitDetected == true)
		{
			for (auto const& OutHitResult : OutHitResults)
			{
				ACharacter* DamagedCharacter = Cast<ACharacter>(OutHitResult.GetActor());
				if (IsValid(DamagedCharacter) == true)
				{
					DamagedCharacters.Add(DamagedCharacter);
				}
			}

			for (auto const& DamagedCharacter : DamagedCharacters)
			{
				UGameplayStatics::ApplyDamage(DamagedCharacter, 5.0f, GetController(), this, UDamageType::StaticClass());
			}
		}
	}
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
					&ABaseCharacter::Jump
				);
	
				EnhancedInputComponent->BindAction(
					MyPlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ABaseCharacter::StopJumping
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
					ETriggerEvent::Started,
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
		AddMovementInput(FVector(0.0f, 1.0f, 0.0f), MoveInput);
		if (bInputEnabled)
		{
			// 캐릭터 회전 반영
			ServerRotationYaw = MoveInput > 0.f ? 0.f : 180.f;
			OnRep_ServerRotationYaw();
		}
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
	if (bInputEnabled)
	{
		// bInputEnabled = false;
		// OnRep_InputEnabled();
		//
		// GetCharacterMovement()->SetMovementMode(MOVE_None);
		if (!HasAuthority() && IsLocallyControlled())
		{
			ServerAttack(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
		}
		// PlayMontage(BaseAttackMontage);
		
	}
}

void ABaseCharacter::SpecialAttack(const FInputActionValue& Value)
{
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

void ABaseCharacter::PlayMontage(const TObjectPtr<UAnimMontage>& Montage)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		AnimInstance->StopAllMontages(0.f);
		AnimInstance->Montage_Play(Montage);
	}
}

