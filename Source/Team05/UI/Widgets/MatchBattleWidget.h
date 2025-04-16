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

	// 1인용 바인딩용 함수 (로컬 플레이어 전용)
	UFUNCTION(BlueprintCallable)
	void BindViewModel(UPlayerStatusViewModel* InViewModel);

	// 멀티 ViewModel 초기화 (GameState 기준)
	UFUNCTION(BlueprintCallable)
	void InitViewModelsFromGameState();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPlayerStatusViewModel> ViewModel;
};
