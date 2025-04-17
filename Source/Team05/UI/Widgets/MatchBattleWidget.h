//MatchBattleWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchBattleWidget.generated.h"

class UTextBlock;
class APS_PlayerState;

UCLASS()
class TEAM05_API UMatchBattleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    // 외부에서 호출해서 상태 업데이트
    UFUNCTION(BlueprintCallable)
    void UpdatePlayerStatus(int32 Index, APS_PlayerState* PlayerState);

protected:
    // 플레이어 1
    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerFatigueRate1;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerLife1;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerMatchHealth1;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerName1;

    // 플레이어 2
    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerFatigueRate2;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerLife2;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerMatchHealth2;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerName2;

    // 플레이어 3
    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerFatigueRate3;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerLife3;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerMatchHealth3;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerName3;

    // 플레이어 4
    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerFatigueRate4;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerLife4;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerMatchHealth4;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* tPlayerName4;

private:
    // 함수 선언
    void BindAllPlayerStates();

};