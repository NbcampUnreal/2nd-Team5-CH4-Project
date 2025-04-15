// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/MatchResult.h"

#include "Components/TextBlock.h"


void UMatchResult::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMatchResult::SetPlayerRankText(int32 RankIndex, const FString& Nickname)
{
    FString FormattedText = FString::Printf(TEXT("%d위: %s"), RankIndex, *Nickname);

    switch (RankIndex)
    {
    case 1:
        if (Rank1Text)
            Rank1Text->SetText(FText::FromString(FormattedText));
        break;
    case 2:
        if (Rank2Text)
            Rank2Text->SetText(FText::FromString(FormattedText));
        break;
    case 3:
        if (Rank3Text)
            Rank3Text->SetText(FText::FromString(FormattedText));
        break;
    case 4:
        if (Rank4Text)
            Rank4Text->SetText(FText::FromString(FormattedText));
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid RankIndex: %d"), RankIndex);
        break;
    }
}


