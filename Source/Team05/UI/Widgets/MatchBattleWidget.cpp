//MatchBattleWidget.cpp

#include "UI/Widgets/MatchBattleWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/Battle/PS_PlayerState.h"

void UMatchBattleWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (const AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>())
    {
        const TArray<APlayerState*>& Players = GS->PlayerArray;

        for (int32 i = 0; i < Players.Num() && i < 4; ++i)
        {
            if (APS_PlayerState* PS = Cast<APS_PlayerState>(Players[i]))
            {
                PS->CachedMatchBattleWidget = this;
                PS->CachedIndex = i;

                UpdatePlayerStatus(i, PS); // 초기 UI 표시
            }
        }
    }
}


void UMatchBattleWidget::UpdatePlayerStatus(int32 Index, APS_PlayerState* PS)
{
    if (!PS) return;

    FString Nickname = PS->GetPlayerNickName();
    FString FatigueStr = FString::Printf(TEXT("%d%%"), PS->GetFatigueRate());
    FString LifeStr = FString::Printf(TEXT("목숨x%d"), PS->GetLife());
    FString HealthStr = FString::Printf(TEXT("%d%%"), PS->GetMatchHealth());

    // 인덱스는 0~3 기준
    switch (Index)
    {
    case 0:
        if (tPlayerName1) tPlayerName1->SetText(FText::FromString(Nickname));
        if (tPlayerFatigueRate1) tPlayerFatigueRate1->SetText(FText::FromString(FatigueStr));
        if (tPlayerLife1) tPlayerLife1->SetText(FText::FromString(LifeStr));
        if (tPlayerMatchHealth1) tPlayerMatchHealth1->SetText(FText::FromString(HealthStr));
        break;
    case 1:
        if (tPlayerName2) tPlayerName2->SetText(FText::FromString(Nickname));
        if (tPlayerFatigueRate2) tPlayerFatigueRate2->SetText(FText::FromString(FatigueStr));
        if (tPlayerLife2) tPlayerLife2->SetText(FText::FromString(LifeStr));
        if (tPlayerMatchHealth2) tPlayerMatchHealth2->SetText(FText::FromString(HealthStr));
        break;
    case 2:
        if (tPlayerName3) tPlayerName3->SetText(FText::FromString(Nickname));
        if (tPlayerFatigueRate3) tPlayerFatigueRate3->SetText(FText::FromString(FatigueStr));
        if (tPlayerLife3) tPlayerLife3->SetText(FText::FromString(LifeStr));
        if (tPlayerMatchHealth3) tPlayerMatchHealth3->SetText(FText::FromString(HealthStr));
        break;
    case 3:
        if (tPlayerName4) tPlayerName4->SetText(FText::FromString(Nickname));
        if (tPlayerFatigueRate4) tPlayerFatigueRate4->SetText(FText::FromString(FatigueStr));
        if (tPlayerLife4) tPlayerLife4->SetText(FText::FromString(LifeStr));
        if (tPlayerMatchHealth4) tPlayerMatchHealth4->SetText(FText::FromString(HealthStr));
        break;
    }
}