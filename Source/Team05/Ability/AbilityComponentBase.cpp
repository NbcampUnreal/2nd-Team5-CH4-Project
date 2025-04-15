#include "AbilityComponentBase.h"

UAbilityComponentBase::UAbilityComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAbilityComponentBase::BeginPlay()
{
	Super::BeginPlay();

}

void UAbilityComponentBase::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAbilityComponentBase::Attack()
{
	
}

void UAbilityComponentBase::UpperAttack()
{
}

void UAbilityComponentBase::LowerAttack()
{
}

void UAbilityComponentBase::FrontAttack()
{
}

void UAbilityComponentBase::SpecialAttack()
{
}

void UAbilityComponentBase::SpecialUpperAttack()
{
}

void UAbilityComponentBase::SpecialLowerAttack()
{
}

void UAbilityComponentBase::SpecialFrontAttack()
{
}



