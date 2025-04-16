//PlayerStatusViewModel.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MVVMViewModelBase.h"
#include "PlayerStatusViewModel.generated.h"

class APS_PlayerState;

UCLASS(BlueprintType)
class TEAM05_API UPlayerStatusViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	UPlayerStatusViewModel();

	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "Player")
	int32 FatigueRate;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "Player")
	int32 Life;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "Player")
	int32 MatchHealth;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "Player")
	FString Nickname;

	void UpdateFromPlayerState(class APS_PlayerState* PlayerState);

};

