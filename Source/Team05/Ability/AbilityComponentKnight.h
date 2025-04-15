#pragma once

#include "CoreMinimal.h"
#include "AbilityComponentBase.h"
#include "SpecialAttack.h"
#include "Components/SceneComponent.h"
#include "AbilityComponentKnight.generated.h"

class ABaseCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEAM05_API UAbilityComponentKnight : public UAbilityComponentBase
{
	GENERATED_BODY()

public:
	UAbilityComponentKnight();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SpecialAttack() override;
	virtual void SpecialUpperAttack() override;
	virtual void SpecialLowerAttack() override;
	virtual void SpecialFrontAttack() override;

	UFUNCTION(Server, Reliable)
	void ServerRPCAttack(UAnimMontage* Montage, float LaunchXDistance = 0.f, float LaunchZDistance = 0.f);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAttack(UAnimMontage* Montage, float LaunchXDistance = 0.f, float LaunchZDistance = 0.f);

	void CheckAttackHit();

	void LaunchCharacter(float LaunchXDistance, float LaunchZDistance);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialUpperAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialLowerAttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> SpecialFrontAttackAnimMontage;
	
	TObjectPtr<ABaseCharacter> OwnerCharacter;
	
	bool bCanAttack;
};
