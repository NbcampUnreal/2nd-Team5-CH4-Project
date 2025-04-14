// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../Character/BaseCharacter.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class TEAM05_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AI|Status")
	bool direction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Status")
	float emoRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Status")
	float atkRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Status")
	float defRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Status")
	float range;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Status")
	float guardTime;
public:
	ABaseAIController();
	UFUNCTION(BlueprintCallable)
	void AIBaseAttack();
	UFUNCTION(BlueprintCallable)
	void AIStartDefense();
	UFUNCTION(BlueprintCallable)
	void AIStopDefense();
	UFUNCTION(BlueprintCallable)
	void AIEmote();
	UFUNCTION(BlueprintCallable)
	void AISkillAttack();
};
