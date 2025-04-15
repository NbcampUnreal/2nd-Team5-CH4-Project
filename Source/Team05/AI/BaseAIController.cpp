// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

#include "Character/KnightCharacter.h"

ABaseAIController::ABaseAIController()
{
	direction = true;
	emoRate = 0.0f;
	atkRate = 1.0f;
	defRate = 0.0f;
	range = 0.0f;
	guardTime = 0.0f;
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

void ABaseAIController::AISkillAttack(FString CharacterName)
{
	APawn* ctrlPawn = GetPawn();
	FVector2D dir;
	if (ctrlPawn)
	{
		if (CharacterName == "Barbarian")
		{
			
		}
		else if (CharacterName == "Mage")
		{
			
		}
		else if (CharacterName == "Rogue")
		{
			
		}
		else // Default: Knight
		{
			AKnightCharacter* ctrlChr;
			ctrlChr = Cast<AKnightCharacter>(ctrlPawn);

			if (ctrlChr)
			{
				dir.X = FMath::FRandRange(0.0f, 1.0f);
				dir.Y = FMath::FRandRange(-1.0f, 1.0f);
				ctrlChr->SetDirection(dir);
				ctrlChr->SpecialAttack();
			}
		}
	}
}

void ABaseAIController::AIStartDefense()
{
	APawn* ctrlPawn = GetPawn();
	FTimerHandle guardTimer;
	if (ctrlPawn)
	{
		ABaseCharacter* ctrlChr;
		ctrlChr = Cast<ABaseCharacter>(ctrlPawn);
		if (ctrlChr)
		{
			ctrlChr->StartGuard();
//			GetWorld()->GetTimerManager().SetTimer(guardTimer, this, &ABaseAIController::AIStopDefense, guardTime, false);
		}
	}
}

void ABaseAIController::AIStopDefense()
{
	APawn* ctrlPawn = GetPawn();
	if (ctrlPawn)
	{
		ABaseCharacter* ctrlChr;
		ctrlChr = Cast<ABaseCharacter>(ctrlPawn);
		if (ctrlChr)
		{
			ctrlChr->StopGuard();
		}
	}
}

void ABaseAIController::AIEmote()
{
	APawn* ctrlPawn = GetPawn();
	if (ctrlPawn)
	{
		ABaseCharacter* ctrlChr;
		ctrlChr = Cast<ABaseCharacter>(ctrlPawn);
		if (ctrlChr)
		{
			ctrlChr->Emote();
		}
	}
}