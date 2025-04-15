#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpecialAttack.h"
#include "AbilityComponentBase.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEAM05_API UAbilityComponentBase : public USceneComponent, public ISpecialAttack
{
	GENERATED_BODY()

public:
	UAbilityComponentBase();

protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	void Attack();
	void UpperAttack();
	void LowerAttack();
	void FrontAttack();

	virtual void SpecialAttack() override;
	virtual void SpecialUpperAttack() override;
	virtual void SpecialLowerAttack() override;
	virtual void SpecialFrontAttack() override;
};
