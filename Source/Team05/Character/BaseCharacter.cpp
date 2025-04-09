// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "MyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Team05/Ability/AbilityComponentKnight.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
	: 
	FatigueRate(0),
	Life(3),
	LastStartAttackTime(0.f),
	AttackTimeDifference(0.f),
	bInputEnabled(true),
	GuardStamina(100),
	MaxGuardStamina(100)
{
	PrimaryActorTick.bCanEverTick = true;
	// 공중에서 좌우 컨트롤 배율 100퍼센트로 지정
	GetCharacterMovement()->AirControl = 1.0f;
	FatigueRate = 0;
	Life = 3;

	AbilityComponent = CreateDefaultSubobject<UAbilityComponentKnight>("Ability Component");

	// 넉백 초기화
	DefaultKnockBackX = 100.f;
	DefaultKnockBackZ = 200.f;
	KnockBackCoefficientX = 10.f;
	KnockBackCoefficientZ = 5.f;
	
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bInputEnabled);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(BaseAttackMontage))
	{
		BaseAttackMontagePlayTime = BaseAttackMontage->GetPlayLength();	
	}
}

void ABaseCharacter::OnRep_TakeDamage()
{
	// 피격 애니메이션 실행

	// 피격 이펙트 실행

	//피격 사운드 실행

	if (CurrentState == STATE_Guard)
	{
		GEngine->AddOnScreenDebugMessage(
		-1,
		5.0f,                  
		FColor::Blue,
		FString::Printf(TEXT("피격되었지만 방어함 피로도: %d"), FatigueRate));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
		-1,
		5.0f,                  
		FColor::Blue,
		FString::Printf(TEXT("피격됨 피로도: %d"), FatigueRate));
	}
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
	if (!HasAuthority() && !IsLocallyControlled())
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
			bInputEnabled = true;
			// OnRep_InputEnabled();
		}), BaseAttackMontagePlayTime - AttackTimeDifference, false, -1.f);
	}

	LastStartAttackTime = InStartAttackTime;
	PlayMontage(BaseAttackMontage);

	MulticastRPCAttack();
}

bool ABaseCharacter::ServerAttack_Validate(float InStartAttackTime)
{
	if (LastStartAttackTime == 0.0f)
	{
		// 최초 공격은 일단 통과.
		return true;
	}
	return (BaseAttackMontagePlayTime - 0.1f) < (InStartAttackTime - LastStartAttackTime);
}

void ABaseCharacter::ServerRotateCharacter_Implementation(const float YawValue)
{
	
	SetActorRotation(FRotator(0.f, YawValue, 0.f));
}


float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority() || CurrentState == STATE_Guard)
	{
		return 0.f;
	}

	const int DamageAmountInt = static_cast<int32>(DamageAmount);
	UE_LOG(LogTemp, Warning, TEXT("%s takes %d damage. %d"), *GetName(), DamageAmountInt, FatigueRate);

	// 넉백 처리
	KnockBack(DamageCauser);
	
	ClientHit();

	FatigueRate += DamageAmountInt;
	return DamageAmount;
}


void ABaseCharacter::CheckAttackHit()
{
	if (HasAuthority())
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
				if (IsValid(DamagedCharacter))
				{
					DamagedCharacters.Add(DamagedCharacter);
				}
			}

			for (auto const& DamagedCharacter : DamagedCharacters)
			{
				UGameplayStatics::ApplyDamage(DamagedCharacter, 5.0f, GetController(), this, UDamageType::StaticClass());
			}

			FColor DrawColor = bIsHitDetected ? FColor::Green : FColor::Red;
			DrawDebugMeleeAttack(DrawColor, Start, End, Forward);
		}
	}
}

void ABaseCharacter::OnRep_GuardState()
{
	if (bOnGuard)
	{
		PlayMontage(GuardMontage);
	}
	else
	{
		StopAnimMontage(GuardMontage);
	}
}

void ABaseCharacter::ServerRPCStartGuard_Implementation()
{
	CurrentState = STATE_Guard;
	bInputEnabled = false;
	bOnGuard = true;
	MulticastRPCChangeGuard(bOnGuard);
}

bool ABaseCharacter::ServerRPCStartGuard_Validate()
{
	if (GuardStamina < 0)
	{
		return false;
	}
	
	return true;
}

void ABaseCharacter::ServerRPCStopGuard_Implementation()
{
	CurrentState = STATE_Idle;
	bInputEnabled = true;
	bOnGuard = false;
	MulticastRPCChangeGuard(bOnGuard);
}

void ABaseCharacter::MulticastRPCChangeGuard_Implementation(bool bGuardState)
{
	if (bGuardState == true)
	{
		if (IsValid(GetWorld()) == true)
		{
			GetWorld()->GetTimerManager().ClearTimer(GuardStaminaTimer);
			GetWorld()->GetTimerManager().SetTimer(GuardStaminaTimer, FTimerDelegate::CreateLambda([&]() -> void
			{
				GuardStamina = FMath::Clamp(GuardStamina - 2, 0, MaxGuardStamina);
				if (GuardStamina <= 0)
				{
					StopGuard();
				}
			}), 0.2f, true);
		}
		
		if (GetMesh()->GetAnimInstance()->GetCurrentActiveMontage() == GuardMontage)
		{
			return;
		}
		PlayMontage(GuardMontage);
	}
	else
	{
		if (IsValid(GetWorld()) == true)
		{
			GetWorld()->GetTimerManager().ClearTimer(GuardStaminaTimer);
			GetWorld()->GetTimerManager().SetTimer(GuardStaminaTimer, FTimerDelegate::CreateLambda([&]() -> void
			{
				GuardStamina = FMath::Clamp(GuardStamina + 2, 0, MaxGuardStamina);
				if (GuardStamina >= MaxGuardStamina)
				{
					GetWorld()->GetTimerManager().ClearTimer(GuardStaminaTimer);
				}
			}), 0.2f, true);
		}
		
		StopAnimMontage(GuardMontage);
	}
}

void ABaseCharacter::ClientHit_Implementation()
{
	PlayMontage(HitMontage);
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
					&ABaseCharacter::Move1D_Input
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
					&ABaseCharacter::SetDirection_Input
				);

				EnhancedInputComponent->BindAction(
					MyPlayerController->DirectionAction,
					ETriggerEvent::Completed,
					this,
					&ABaseCharacter::ResetDirection
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
					ETriggerEvent::Started,
					this,
					&ABaseCharacter::SpecialAttack
				);
			}
	
			// 캐릭터 특수 이동 바인딩
			if (MyPlayerController->SpecialMoveAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->SpecialMoveAction,
					ETriggerEvent::Started,
					this,
					&ABaseCharacter::SpecialMove
				);
			}
	
			// 캐릭터 가드 바인딩
			if (MyPlayerController->GuardAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->GuardAction,
					ETriggerEvent::Started,
					this,
					&ABaseCharacter::StartGuard
				);
			}
			if (MyPlayerController->GuardAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->GuardAction,
					ETriggerEvent::Completed,
					this,
					&ABaseCharacter::StopGuard
				);
			}
	
			// 캐릭터 이모지 바인딩
			if (MyPlayerController->EmoteAction)
			{
				EnhancedInputComponent->BindAction(
					MyPlayerController->EmoteAction,
					ETriggerEvent::Started,
					this,
					&ABaseCharacter::Emote
				);
			}
		}
	}
}

void ABaseCharacter::Move1D_Input(const FInputActionValue& Value)
{
	
	if (!Controller) return;

	if (const float MoveInput = Value.Get<float>(); !FMath::IsNearlyZero(MoveInput))
	{
		Move1D(MoveInput);
	}
}

void ABaseCharacter::SetDirection_Input(const FInputActionValue& Value)
{
	const FVector2D InputDirection = Value.Get<FVector2D>();

	SetDirection(InputDirection);
}

void ABaseCharacter::Move1D(const float Value)
{
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
	if (bInputEnabled)
	{
		const float Yaw = Value > 0.f ? 0.f : 180.f;

		// 회전 적용
		GetController()->SetControlRotation(FRotator(0.0f, Yaw, 0.0f));
	}
}

void ABaseCharacter::SetDirection(const FVector2D Value)
{
	// 키보드 입력을 구분하기 위한 각도 계산
	// 위   90   (x = 0, y = 1)
	// 아래 -90  (x = 0, y = 0)
	// 앞   0    (x = 1, u = 0)
	const float AngleDegrees = FMath::RadiansToDegrees(FMath::Atan2(Value.X, Value.Y));
	
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

void ABaseCharacter::ResetDirection()
{
	CurrentDirection = CurrentDirection = EDirectionEnum::ENone;
}

void ABaseCharacter::BaseAttack()
{
	if (bInputEnabled)
	{
		ServerAttack(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
		PlayMontage(BaseAttackMontage);
	}
}

void ABaseCharacter::SpecialAttack()
{
	if (bInputEnabled)
	{
		AbilityComponent->SpecialAttack();
	}
}

void ABaseCharacter::SpecialMove()
{
	
}

void ABaseCharacter::StartGuard()
{
	if (GuardStamina < 10)
	{
		return;
	}

	PlayMontage(GuardMontage);
	ServerRPCStartGuard();
}

void ABaseCharacter::StopGuard()
{
	StopAnimMontage(GuardMontage);
	
	ServerRPCStopGuard();
}

void ABaseCharacter::Emote()
{
	PlayMontage(EmoteMontage);
}

void ABaseCharacter::KnockBack(const AActor* DamageCauser)
{
	float XDirection = GetActorLocation().X - DamageCauser->GetActorLocation().X;
	XDirection = (XDirection >= 0.f) ? 1.f : -1.f;
	
	const int32 KnockBackAmount = DefaultKnockBackX + KnockBackCoefficientX * FatigueRate;
	const FVector KnockBackDir = FVector(XDirection, 0.f, 0.f);

	FVector LaunchVelocity = KnockBackDir * KnockBackAmount;
	LaunchVelocity.Z = DefaultKnockBackZ + KnockBackCoefficientZ * FatigueRate;
	
	LaunchCharacter(LaunchVelocity, true, true);
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

void ABaseCharacter::DrawDebugMeleeAttack(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd,
	FVector Forward)
{
	const float MeleeAttackRange = 50.f;
	const float MeleeAttackRadius = 50.f;
	FVector CapsuleOrigin = TraceStart + (TraceEnd - TraceStart) * 0.5f;
	float CapsuleHalfHeight = MeleeAttackRange * 0.5f;
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, MeleeAttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 5.0f);
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	// 부모 클래스 로직을 먼저 실행
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		bInputEnabled = true;
		OnRep_InputEnabled();
	}
}

void ABaseCharacter::OnRep_Owner()
{
	// 부모 클래스의 처리 실행
	Super::OnRep_Owner();
}

void ABaseCharacter::PostNetInit()
{
	Super::PostNetInit();
}
