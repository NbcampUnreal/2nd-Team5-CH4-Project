// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

ABaseAIController::ABaseAIController()
{
	direction = true;
	emoRate = 0.0f;
	atkRate = 1.0f;
	defRate = 0.0f;
	range = 0.0f;
	
}

void ABaseAIController::AIBaseAttack()
{
	APawn* ctrlPawn= GetPawn();
	if (ctrlPawn)
	{
		ABaseCharacter* ctrlChr;
		ctrlChr = Cast<ABaseCharacter>(ctrlPawn);
		if (ctrlChr)
		{
			ctrlChr->BaseAttack();
		}
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