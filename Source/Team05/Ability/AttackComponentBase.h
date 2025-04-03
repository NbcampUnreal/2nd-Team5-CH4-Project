#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpecialAttack.h"
#include "AttackComponentBase.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEAM05_API UAttackComponentBase : public UActorComponent, public ISpecialAttack
{
	GENERATED_BODY()

public:
	UAttackComponentBase();

protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void UpperAttack() override;

	virtual void LowerAttack() override;

	virtual void FrontAttack() override;
};
