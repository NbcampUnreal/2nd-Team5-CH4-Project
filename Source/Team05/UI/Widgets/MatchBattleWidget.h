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
	// 플레이어 4명의 ViewModel 저장
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ViewModel", meta = (BindWidgetViewModel))
	TObjectPtr<UPlayerStatusViewModel> ViewModel1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ViewModel", meta = (BindWidgetViewModel))
	TObjectPtr<UPlayerStatusViewModel> ViewModel2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ViewModel", meta = (BindWidgetViewModel))
	TObjectPtr<UPlayerStatusViewModel> ViewModel3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ViewModel", meta = (BindWidgetViewModel))
	TObjectPtr<UPlayerStatusViewModel> ViewModel4;

	// 외부에서 ViewModel을 인덱스별로 할당할 수 있는 함수
	UFUNCTION(BlueprintCallable, Category = "ViewModel")
	void SetViewModelAt(int32 Index, UPlayerStatusViewModel* InViewModel);
};
