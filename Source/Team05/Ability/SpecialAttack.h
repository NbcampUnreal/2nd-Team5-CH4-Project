#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpecialAttack.generated.h"

UINTERFACE()
class USpecialAttack : public UInterface
{
	GENERATED_BODY()
};

class TEAM05_API ISpecialAttack
{
	GENERATED_BODY()

public:
	virtual void UpperAttack() = 0;

	virtual void LowerAttack() = 0;

	virtual void FrontAttack() = 0;
};
