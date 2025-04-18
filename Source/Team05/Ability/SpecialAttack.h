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
	virtual void SpecialAttack() = 0;
	
	virtual void SpecialUpperAttack() = 0;

	virtual void SpecialLowerAttack() = 0;

	virtual void SpecialFrontAttack() = 0;
};
