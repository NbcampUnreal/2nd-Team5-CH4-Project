#include "AttackComponentBase.h"

UAttackComponentBase::UAttackComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAttackComponentBase::BeginPlay()
{
	Super::BeginPlay();

}

void UAttackComponentBase::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttackComponentBase::UpperAttack()
{
	
}

void UAttackComponentBase::LowerAttack()
{
	
}

void UAttackComponentBase::FrontAttack()
{
	
}

