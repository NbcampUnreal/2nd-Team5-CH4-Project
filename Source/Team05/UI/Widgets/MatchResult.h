// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchResult.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class TEAM05_API UMatchResult : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 순위 및 닉네임 설정 함수
    UFUNCTION(BlueprintCallable, Category = "MatchResult")
    void SetPlayerRankText(int32 RankIndex, const FString& Nickname);

protected:
    virtual void NativeConstruct() override;

    // 바인딩된 텍스트 위젯들 (블루프린트에서 변수 이름으로 바인딩 필수)
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rank1Text;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rank2Text;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rank3Text;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rank4Text;
};
