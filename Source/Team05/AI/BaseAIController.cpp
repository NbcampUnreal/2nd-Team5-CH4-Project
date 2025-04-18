// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"
#include "Components/SphereComponent.h"
#include "Character/MageCharacter.h"
#include "Character/RogueCharacter.h"
#include "Character/KnightCharacter.h"
#include "Character/BarbarianCharacter.h"

ABaseAIController::ABaseAIController()
{
	direction = true;
	emoRate = 0.0f;
	atkRate = 1.0f;
	defRate = 0.0f;
	range = 0.0f;
	guardTime = 0.0f;
	bReplicates = true;
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
	float skillRate = FMath::FRandRange(0.0f, 1.0f);
	//FVector2D dir;
	if (ctrlPawn)
	{
		if (CharacterName == "Barbarian")
		{
			ABarbarianCharacter* ctrlChr;
			ctrlChr = Cast<ABarbarianCharacter>(ctrlPawn);

			if (ctrlChr)
			{
				//dir.X = FMath::FRandRange(0.0f, 1.0f);
				//dir.Y = FMath::FRandRange(-1.0f, 1.0f);
				//ctrlChr->SetDirection(dir);
				if (skillRate < 0.25f)
				{
					ctrlChr->SpecialAttack();
				}
				else if (skillRate >= 0.25f && skillRate < 0.5f)
				{
					ctrlChr->SpecialFrontAttack();
				}
				else if (skillRate >= 0.5f && skillRate < 0.75f)
				{
					ctrlChr->SpecialLowerAttack();
				}
				else
				{
					ctrlChr->SpecialUpperAttack();
				}
			}
		}
		else if (CharacterName == "Mage")
		{
			AMageCharacter* ctrlChr;
			ctrlChr = Cast<AMageCharacter>(ctrlPawn);

			if (ctrlChr)
			{
				ctrlChr->SpecialAttack();
			}
		}
		else if (CharacterName == "Rogue")
		{
			ARogueCharacter* ctrlChr;
			ctrlChr = Cast<ARogueCharacter>(ctrlPawn);

			if (ctrlChr)
			{
				ctrlChr->SpecialAttack();
			}
		}
		else // Default: Knight
		{
			AKnightCharacter* ctrlChr;
			ctrlChr = Cast<AKnightCharacter>(ctrlPawn);

			if (ctrlChr)
			{
				//dir.X = FMath::FRandRange(0.0f, 1.0f);
				//dir.Y = FMath::FRandRange(-1.0f, 1.0f);
				//ctrlChr->SetDirection(dir);
				if (skillRate < 0.25f)
				{
					ctrlChr->SpecialAttack();
				}
				else if (skillRate >= 0.25f && skillRate < 0.5f)
				{
					ctrlChr->SpecialFrontAttack();
				}
				else if (skillRate >= 0.5f && skillRate < 0.75f)
				{
					ctrlChr->SpecialLowerAttack();
				}
				else
				{
					ctrlChr->SpecialUpperAttack();
				}
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

AActor* ABaseAIController::AIDetectTarget()
{
	ABaseCharacter* chr = Cast<ABaseCharacter>(GetPawn());
	USphereComponent* detectSphere=chr->GetDetectSphere();
	float closestDistance = 10000.0f;
	AActor* closestChr=nullptr;

	if (chr && detectSphere)
	{
		TArray<AActor*> overlappingActors;
		detectSphere->GetOverlappingActors(overlappingActors, ABaseCharacter::StaticClass());

		for (AActor* act : overlappingActors)
		{			
			if (act != chr &&  closestDistance > FVector::Distance(act->GetActorLocation(), chr->GetActorLocation()))
			{
				closestDistance = FVector::Distance(act->GetActorLocation(), chr->GetActorLocation());
				closestChr = act;
			}
		}
	}

	return closestChr;
}