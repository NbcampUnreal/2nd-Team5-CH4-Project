//MageCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Ability/SpecialAttack.h"
#include "MageProjectile.h"
#include "MageCharacter.generated.h"

UCLASS()
class TEAM05_API AMageCharacter : public ABaseCharacter, public ISpecialAttack
{
	GENERATED_BODY()

public:
	AMageCharacter();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SpecialAttack_Input();
	virtual void SpecialAttack() override;
	virtual void SpecialUpperAttack() override;
	virtual void SpecialLowerAttack() override;
	virtual void SpecialFrontAttack() override;

	// 투사체 발사
	UFUNCTION(Server, Reliable)
	void Fire();

	// 발사체 생성 위치 차이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialUpperAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialLowerAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialFrontAttackAnimMontage;

	// 발사체 클래스
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AMageProjectile> ProjectileClass;
};
