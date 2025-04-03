// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialAttack.h"
#include "Components/SceneComponent.h"
#include "AbilityComponentKnight.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEAM05_API UAbilityComponentKnight : public USceneComponent, public ISpecialAttack
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAbilityComponentKnight();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SpecialAttack() override;
	virtual void SpecialUpperAttack() override;
	virtual void SpecialLowerAttack() override;
	virtual void SpecialFrontAttack() override;
};
