// Fill out your copyright notice in the Description page of Project Settings.


#include "FireProjectileNotify.h"

#include "Character/BaseCharacter.h"
#include "Character/MageCharacter.h"
#include "Character/RogueCharacter.h"

void UFireProjectileNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner());

	if (OwnerCharacter->IsA(ARogueCharacter::StaticClass()))
	{
		ARogueCharacter* Rogue = Cast<ARogueCharacter>(OwnerCharacter);
		if (IsValid(Rogue))
		{
			Rogue->Fire();
		}
	}
	else if (OwnerCharacter->IsA(AMageCharacter::StaticClass()))
	{
		AMageCharacter* Mage = Cast<AMageCharacter>(OwnerCharacter);
		if (IsValid(Mage))
		{
			Mage->Fire();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong type of character. Character must be a Mage or Rogue."));
	}
}
