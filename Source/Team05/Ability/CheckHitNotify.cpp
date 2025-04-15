#include "CheckHitNotify.h"
#include "Character/BaseCharacter.h"

void UCheckHitNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacter->CheckAttackHit(AttackDamage, AttackRange, AttackStartDistance);
	}
}
