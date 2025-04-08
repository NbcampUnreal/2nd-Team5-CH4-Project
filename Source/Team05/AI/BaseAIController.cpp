// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

ABaseAIController::ABaseAIController()
{
	direction = true;
	emoRate = 0.0f;
	atkRate = 1.0f;
	defRate = 0.0f;
	range = 0.0f;
	ctrlPawn = GetPawn();
	ctrlChr = nullptr;
	if (ctrlPawn)
	{
		ctrlChr = Cast<ABaseCharacter>(ctrlPawn);		
	}
}

void ABaseAIController::AIBaseAttack()
{
	if (ctrlChr)
	{
		//ctrlChr->BaseAttack();
	}
}

void ABaseAIController::AISkillAttack()
{

}

void ABaseAIController::AIDefense()
{

}

void ABaseAIController::AIEmote()
{

}

void ABaseAIController::FindTarget()
{

}