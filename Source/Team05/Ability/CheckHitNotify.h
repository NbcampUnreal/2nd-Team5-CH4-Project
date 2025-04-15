// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CheckHitNotify.generated.h"

UCLASS()
class TEAM05_API UCheckHitNotify : public UAnimNotify
{
	GENERATED_BODY()

private:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CheckHit")
	float AttackDamage;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CheckHit")
	float AttackRange;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "CheckHit")
	float AttackStartDistance;
};
