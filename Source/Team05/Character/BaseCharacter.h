//BaseCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "UI/Widgets/NameTagWidget.h"    
#include "BaseCharacter.generated.h"

class UInputAction;
struct FInputActionValue;

UENUM()
enum EDirectionEnum
{
	ENone,
	EForward,
	EUp,
	EDown,
};

UENUM()
enum ECharacterState
{
    STATE_Idle,
	STATE_Attack,
	STATE_Guard,
	State_Hit,
	STATE_Respawn
};


UCLASS()
class TEAM05_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 몽타주 리스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> BaseAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> HitMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> GuardMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> EmoteMontage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 피로도 (높을 수록 멀리 타격됨, 매치마다 0으로 초기화)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 FatigueRate;
	// 생명 (매치마다 3개로 초기화)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 Life;
	// 부활 시 무적 시간
	float RespawnImmunityTime;
	// 현재 키보드 입력 방향
	EDirectionEnum CurrentDirection;

	// 넉백 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnockBack")
	float DefaultKnockBackX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnockBack")
	float DefaultKnockBackZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnockBack")
	float KnockBackCoefficientX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnockBack")
	float KnockBackCoefficientZ;

	// 서버

	// 플레이어 입력 가능 여부
	// 공격, 특수 공격, 가드 중 다른 입력 막기 (bInputEnabled = true)
	// 각 애니메이션이 끝났을 때 replication 됨
	UPROPERTY(ReplicatedUsing = OnRep_InputEnabled)
	uint8 bInputEnabled : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpecialAttackCooldown;
	bool bCanSpecialAttack;
	FTimerHandle SpecialAttackCooldownTimer;
	
	ECharacterState CurrentState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxJumpCount;

#pragma region Guard
	uint8 bOnGuard : 1;
	UPROPERTY(Replicated)
	int32 GuardStamina;
	int32 MaxGuardStamina;
	FVector CurrentGuardScale;
	FTimerHandle GuardStaminaTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guard")
	UStaticMeshComponent* GuardSphere;

	UFUNCTION(Server, Reliable)
	void ServerRPCStartGuard();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCStopGuard();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCChangeGuard(bool bGuardState);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCApplyGuardSphereSize(float DeltaTime);
	
#pragma endregion
	
	UFUNCTION()
	void OnRep_InputEnabled();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCAttack(UAnimMontage* AnimMontage);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCAttack(UAnimMontage* AnimMontage);

	UFUNCTION(Server, Reliable)
	void ServerRPCRotateCharacter(float YawValue);

	// 캐릭터 대시
	UFUNCTION(Server, Reliable)
	void Launch(const float LaunchXDistance, const float LaunchZDistance);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHit();

	void SetCooldownTimer(float Cooldown);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	void CheckAttackHit(float Damage, float AttackRange, bool bAttackFromOrigin);

	void ReduceLife();
	int32 GetLife() const { return Life; }
	// 부활 후 무적 처리
	void RespawnImmunity();
	
	UFUNCTION()
	void Move1D_Input(const FInputActionValue& Value);
	UFUNCTION()
	void SetDirection_Input(const FInputActionValue& Value);

	// 캐릭터 행동 시작
	// AI에서 그대로 사용
	UFUNCTION()
	void Move1D(const float Value);
	UFUNCTION()
	void SetDirection(FVector2D Value);
	UFUNCTION()
	void ResetDirection();
	UFUNCTION()
	void BaseAttack();
	UFUNCTION()
	virtual void SpecialMove();
	UFUNCTION()
	void StartGuard();
	UFUNCTION()
	void StopGuard();
	UFUNCTION()
	void Emote();
	// 캐릭터 행동 끝

	void KnockBack(const AActor* DamageCauser);

	void HitImmunity();
	
	void PlayMontage(const TObjectPtr<UAnimMontage>& Montage);

	void DrawDebugMeleeAttack(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward);

	//서버 관련
	// 서버에서 이 캐릭터를 특정 컨트롤러가 점유(Possess)할 때 호출됩니다.
	virtual void PossessedBy(AController* NewController) override;

	// 클라이언트에서 Owner가 복제될 때 호출됩니다.
	virtual void OnRep_Owner() override;

	// 네트워크 초기화 이후 호출됩니다. (PostReplication 관련)
	virtual void PostNetInit() override;


	// UI
	// NameTag 위젯
public:
	UFUNCTION()
	void UpdateNameTagUI(const FString& NewNickname);

	UFUNCTION()
	USphereComponent* GetDetectSphere();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UWidgetComponent> NameTagComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIDetection")//AI detection radius
	class USphereComponent* DetectionSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AICharacterName")
	FString AIchrName;
private:
	bool bNameTagBound = false;
};
