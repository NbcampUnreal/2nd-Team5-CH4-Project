//BaseCharacter.cpp

#include "BaseCharacter.h"

#include "MyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "UI/Widgets/NameTagWidget.h"
#include "GameModes/Battle/PS_PlayerState.h"
#include "GameModes/Battle/GM_BattleMode.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
	: 
	FatigueRate(0),
	Life(3),
	bInputEnabled(true),
	MaxJumpCount(2),
	GuardStamina(100),
	MaxGuardStamina(100)
{
	PrimaryActorTick.bCanEverTick = true;
	// 공중에서 좌우 컨트롤 배율 100퍼센트로 지정
	GetCharacterMovement()->AirControl = 1.0f;
	FatigueRate = 0;
	Life = 3;

	// 리스폰 무적 시간 초기화
	RespawnImmunityTime = 1.f;

	// 넉백 초기화
	DefaultKnockBackX = 100.f;
	DefaultKnockBackZ = 200.f;
	KnockBackCoefficientX = 10.f;
	KnockBackCoefficientZ = 5.f;

	Super::JumpMaxCount = MaxJumpCount;
	GetCharacterMovement()->JumpZVelocity = 600.f;

	bCanSpecialAttack = true;
	SpecialAttackCooldown = 3.f;

	
	// 가드 스피어 초기화
	GuardSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GuardSphere"));
	GuardSphere->SetupAttachment(RootComponent);
	GuardSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GuardSphere->SetVisibility(false);

	// 가드 스피어 Mesh 설정
	static ConstructorHelpers::FObjectFinder<UStaticMesh>GuardSphereMesh(TEXT("/Game/_Blueprint/Character/Meshes/SM_GuardSphere.SM_GuardSphere"));
	if(GuardSphereMesh.Succeeded())
	{
		GuardSphere->SetStaticMesh(GuardSphereMesh.Object);
	}
	
	CurrentGuardScale = FVector(1.0f);

	//UI-NameTag
	NameTagComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameTagComponent"));
	NameTagComponent->SetupAttachment(RootComponent);
	NameTagComponent->SetWidgetSpace(EWidgetSpace::Screen);
	NameTagComponent->SetDrawAtDesiredSize(true);
	NameTagComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f)); 

	//AI detect sphere
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetSphereRadius(1000.0f);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	//AI chr Name
	AIchrName = "";
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bInputEnabled);
	DOREPLIFETIME(ABaseCharacter, GuardStamina);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GuardSphere->SetTranslucentSortPriority(100);
}

void ABaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(GuardStaminaTimer);
	GetWorldTimerManager().ClearTimer(SpecialAttackCooldownTimer);
	GuardStaminaTimer.Invalidate();
	SpecialAttackCooldownTimer.Invalidate();
}

void ABaseCharacter::MulticastRPCApplyGuardSphereSize_Implementation(float DeltaTime)
{
	float GuardRatio = static_cast<float>(GuardStamina) / static_cast<float>(MaxGuardStamina);
	FVector TargetScale = FVector(GuardRatio);

	CurrentGuardScale = FMath::VInterpTo(CurrentGuardScale, TargetScale, DeltaTime, 5.0f);
	GuardSphere->SetRelativeScale3D(CurrentGuardScale);
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

void ABaseCharacter::ServerRPCAttack_Implementation(UAnimMontage* AnimMontage)
{
	float MontagePlayTime = AnimMontage->GetPlayLength();
	bInputEnabled = false;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		bInputEnabled = true;
	}), MontagePlayTime, false, -1.f);
	PlayMontage(AnimMontage);
	MulticastRPCAttack(AnimMontage);
}

void ABaseCharacter::MulticastRPCAttack_Implementation(UAnimMontage* AnimMontage)
{
	if (!HasAuthority() && !IsLocallyControlled())
	{
		PlayMontage(AnimMontage);
	}
}

void ABaseCharacter::ServerRPCRotateCharacter_Implementation(const float YawValue)
{
	
	SetActorRotation(FRotator(0.f, YawValue, 0.f));
}


float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority() || CurrentState == ECharacterState::STATE_Guard || CurrentState == ECharacterState::STATE_Hit || CurrentState == ECharacterState::STATE_Respawn)
	{
		return 0.f;
	}

	const int DamageAmountInt = static_cast<int32>(DamageAmount);
	UE_LOG(LogTemp, Warning, TEXT("%s takes %d damage."), *GetName(), DamageAmountInt);

	// 피격 후 면역 처리
	HitImmunity();
	// 넉백 처리
	KnockBack(DamageCauser);

	MulticastRPCHit();

	// 1. BaseCharacter 내부에 누적
	FatigueRate += DamageAmountInt;

	// 2. PlayerState로 UI 반영용 값 복사
	if (APS_PlayerState* PS = GetPlayerState<APS_PlayerState>())
	{
		PS->SetFatigueRate(FatigueRate);
	}

	return DamageAmount;
}


void ABaseCharacter::CheckAttackHit(float AttackDamage, float AttackRange, bool bAttackFromOrigin)
{
	if (HasAuthority())
	{
		TArray<FHitResult> OutHitResults;
		TSet<ACharacter*> DamagedCharacters;
		FCollisionQueryParams Params(NAME_None, false, this);
		
		const float AttackRadius = 50.f;
		const FVector Forward = GetActorForwardVector();
		FVector Start, End;
		if (bAttackFromOrigin == true)
		{
			Start = GetActorLocation() - Forward * AttackRange;
			End = GetActorLocation() + Forward * AttackRange;
		}
		else
		{
			Start = GetActorLocation() + Forward * GetCapsuleComponent()->GetScaledCapsuleRadius();
			End = Start + Forward * AttackRange;
		}

		bool bIsHitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, Start, End, FQuat::Identity, ECC_Camera, FCollisionShape::MakeSphere(AttackRadius), Params);
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
				UGameplayStatics::ApplyDamage(DamagedCharacter, AttackDamage, GetController(), this, UDamageType::StaticClass());
			}

			FColor DrawColor = bIsHitDetected ? FColor::Green : FColor::Red;
			DrawDebugMeleeAttack(DrawColor, Start, End, Forward);
		}
	}
}

void ABaseCharacter::ReduceLife()
{
	// 1. BaseCharacter 내부 변수 조정
	Life = FMath::Clamp(Life - 1, 0, 3);
	FatigueRate = 0;

	// 2. PlayerState로 값 복사
	if (APS_PlayerState* PS = GetPlayerState<APS_PlayerState>())
	{
		PS->SetLife(Life);
		PS->SetFatigueRate(FatigueRate);
	}
}

void ABaseCharacter::RespawnImmunity()
{
	// 리스폰 이펙트 실행
	
	CurrentState = ECharacterState::STATE_Respawn;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		CurrentState = ECharacterState::STATE_Idle;
	}), RespawnImmunityTime, false, -1.f);
}


void ABaseCharacter::ServerRPCStartGuard_Implementation()
{	
	CurrentState = ECharacterState::STATE_Guard;
	bInputEnabled = false;
	bOnGuard = true;

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
	
	MulticastRPCChangeGuard(bOnGuard);
}

void ABaseCharacter::ServerRPCStopGuard_Implementation()
{
	CurrentState = ECharacterState::STATE_Idle;
	bInputEnabled = true;
	bOnGuard = false;

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
	MulticastRPCChangeGuard(bOnGuard);
}

void ABaseCharacter::MulticastRPCChangeGuard_Implementation(bool bGuardState)
{
	if (HasAuthority() == true)
	{
		return;
	}

	if (bGuardState == true)
	{
		GuardSphere->SetVisibility(true);
		if (GetMesh()->GetAnimInstance()->GetCurrentActiveMontage() == GuardMontage)
		{
			return;
		}
		PlayMontage(GuardMontage);
	}
	else
	{
		GuardSphere->SetVisibility(false);
		StopAnimMontage(GuardMontage);
	}
}

void ABaseCharacter::MulticastRPCHit_Implementation()
{
	PlayMontage(HitMontage);
	// 피격 후 면역 처리
	HitImmunity();
}

void ABaseCharacter::SetCooldownTimer(float Cooldown)
{
	bCanSpecialAttack = false;
	if (IsValid(GetWorld()) == true)
	{
		GetWorldTimerManager().SetTimer(SpecialAttackCooldownTimer, FTimerDelegate::CreateLambda([&]()
		{
			bCanSpecialAttack = true;
		}), Cooldown, false);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 가드 크기 변화
	if (bOnGuard)
	{
		MulticastRPCApplyGuardSphereSize(DeltaTime);
	}
	// 최초 1회만 닉네임 바인딩
	if (!bNameTagBound)
	{
		if (APS_PlayerState* PS = GetPlayerState<APS_PlayerState>())
		{
			const FString& Nick = PS->GetPlayerNickName();
			if (!Nick.IsEmpty())
			{
				UpdateNameTagUI(Nick);
				bNameTagBound = true; // 한 번만 처리
			}
		}
	}
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
					ETriggerEvent::Started,
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
	// 위   0   (x = 0, y = 1)
	// 앞   90    (x = 1, u = 0)
	// 아래 180  (x = 0, y = -1)
	float AngleDegrees = FMath::RadiansToDegrees(FMath::Atan2(Value.X, Value.Y));
	if (AngleDegrees < 0.f) AngleDegrees += 360.f;
	
	const float CurrentYaw = GetController()->GetControlRotation().Yaw;

	if (AngleDegrees > -45.0f && AngleDegrees <= 45.0f)
	{
		CurrentDirection = EDirectionEnum::EUp;
	}
	else if ((AngleDegrees > 45.0f && AngleDegrees <= 135.0f && FMath::IsNearlyEqual(CurrentYaw , 0.f))
		|| (AngleDegrees > 225.0f && AngleDegrees <= 315.0f && FMath::IsNearlyEqual(CurrentYaw , 180.f))) 
	{
		CurrentDirection = EDirectionEnum::EForward;
	}
	else if (AngleDegrees >= 135.0f && AngleDegrees < 225.f)
	{
		CurrentDirection = EDirectionEnum::EDown;
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
		ServerRPCAttack(BaseAttackMontage);
		PlayMontage(BaseAttackMontage);
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

	// 가드 스피어 활성화
	GuardSphere->SetVisibility(true);
	
	PlayMontage(GuardMontage);
	ServerRPCStartGuard();
}

void ABaseCharacter::StopGuard()
{
	// 가드 스피어 비활성화
	GuardSphere->SetVisibility(false);
	
	StopAnimMontage(GuardMontage);
	
	ServerRPCStopGuard();
}

void ABaseCharacter::Emote()
{
	PlayMontage(EmoteMontage);
}

void ABaseCharacter::Launch_Implementation(const float LaunchXDistance, const float LaunchZDistance)
{
	float XDirection = GetActorForwardVector().X;
	XDirection = (XDirection >= 0.f) ? 1.f : -1.f;
	const FVector LaunchVelocity = FVector(XDirection * LaunchXDistance, 0.f, 200.f + LaunchZDistance);
	LaunchCharacter(LaunchVelocity, true, true);
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

void ABaseCharacter::HitImmunity()
{
	// Hit로 현재 상태 변경
	// 타이머로 HitMontage 실행 시간만큼 뒤에 현재 상태를 Idle 상태로 전환
	CurrentState = ECharacterState::STATE_Hit;
	const float HitMontagePlayTime = HitMontage->GetPlayLength();
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			CurrentState = ECharacterState::STATE_Idle;
		}), HitMontagePlayTime, false, -1.f);
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

//UI-NameTag
void ABaseCharacter::UpdateNameTagUI(const FString& NewNickname)
{
	if (NameTagComponent && NameTagComponent->GetUserWidgetObject())
	{
		if (UNameTagWidget* Widget = Cast<UNameTagWidget>(NameTagComponent->GetUserWidgetObject()))
		{
			Widget->SetNickname(NewNickname);
		}
	}
}

USphereComponent* ABaseCharacter::GetDetectSphere()
{
	return DetectionSphere;
}