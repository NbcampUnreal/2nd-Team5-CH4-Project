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

   // 체력 (0~100)
	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "ViewModel")
	int32 MatchHealth;

	// 피로도 (0~100)
	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "ViewModel")
	int32 FatigueRate;

	// 목숨 수 (기본 3)
	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "ViewModel")
	int32 Life;

	// 닉네임
	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "ViewModel")
	FString Nickname;

};

