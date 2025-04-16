//MatchBattleWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Viewmodel/PlayerStatusViewModel.h"
#include "MatchBattleWidget.generated.h"


UCLASS()
class TEAM05_API UMatchBattleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 플레이어 4명의 ViewModel 보관
	UPROPERTY(BlueprintReadWrite, Category = "ViewModel")
	TObjectPtr<UPlayerStatusViewModel> ViewModel1;

	UPROPERTY(BlueprintReadWrite, Category = "ViewModel")
	TObjectPtr<UPlayerStatusViewModel> ViewModel2;

	UPROPERTY(BlueprintReadWrite, Category = "ViewModel")
	TObjectPtr<UPlayerStatusViewModel> ViewModel3;

	UPROPERTY(BlueprintReadWrite, Category = "ViewModel")
	TObjectPtr<UPlayerStatusViewModel> ViewModel4;

	// 외부에서 각 인덱스에 ViewModel을 세팅
	UFUNCTION(BlueprintCallable, Category = "ViewModel")
	void SetViewModelAt(int32 Index, UPlayerStatusViewModel* InViewModel);
};
