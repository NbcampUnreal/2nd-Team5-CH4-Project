//MatchBattleWidget.cpp

#include "UI/Widgets/MatchBattleWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "TimerManager.h"
#include "GameModes/Battle/GS_BattleState.h"
#include "GameModes/Battle/PS_PlayerState.h"

void UMatchBattleWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // UI 생성 후 한 프레임 뒤에 PlayerArray 연결
    FTimerHandle DelayHandle;
    GetWorld()->GetTimerManager().SetTimer(DelayHandle, this, &UMatchBattleWidget::BindAllPlayerStates, 1.0f, false);
    // 위젯 생성 후 GameState에 자기 자신 등록
    if (AGS_BattleState* GS = GetWorld()->GetGameState<AGS_BattleState>())
    {
        GS->RegisterMatchWidget(this);
    }

}

void UMatchBattleWidget::BindAllPlayerStates()
{
    const AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
    if (!GS)
    {
        UE_LOG(LogTemp, Warning, TEXT("BindAllPlayerStates: GameState is null"));
        return;
    }

    const TArray<APlayerState*>& Players = GS->PlayerArray;
    UE_LOG(LogTemp, Warning, TEXT("BindAllPlayerStates: PlayerArray.Num() = %d"), Players.Num());

    for (int32 i = 0; i < Players.Num() && i < 4; ++i)
    {
        if (APS_PlayerState* PS = Cast<APS_PlayerState>(Players[i]))
        {
            PS->CachedMatchBattleWidget = this;
            PS->CachedIndex = i;
            UpdatePlayerStatus(i, PS);
        }
    }
}


void UMatchBattleWidget::UpdatePlayerStatus(int32 Index, APS_PlayerState* PS)
{
    if (!PS) return;

    APawn* Pawn = PS->GetPawn();
    if (!Pawn || !Pawn->IsPlayerControlled())
    {
        // AI는 표시하지 않음
        UE_LOG(LogTemp, Log, TEXT("Index %d은 AI이므로 UI에서 생략"), Index);
        return;
    }

    FString Nickname = PS->GetPlayerNickName();
    FString FatigueStr = FString::Printf(TEXT("%d%%"), PS->GetFatigueRate());
    FString LifeStr = FString::Printf(TEXT("목숨x%d"), PS->GetLife());
    UE_LOG(LogTemp, Log, TEXT("Update UI [Index %d] Nickname=%s HP=%s Fatigue=%s"),
        Index, *Nickname, *LifeStr, *FatigueStr);

    // 인덱스는 0~3 기준
    switch (Index)
    {
    case 0:
        if (tPlayerName1) tPlayerName1->SetText(FText::FromString(Nickname));
        if (tPlayerFatigueRate1) tPlayerFatigueRate1->SetText(FText::FromString(FatigueStr));
        if (tPlayerLife1) tPlayerLife1->SetText(FText::FromString(LifeStr));
        break;
    case 1:
        if (tPlayerName2) tPlayerName2->SetText(FText::FromString(Nickname));
        if (tPlayerFatigueRate2) tPlayerFatigueRate2->SetText(FText::FromString(FatigueStr));
        if (tPlayerLife2) tPlayerLife2->SetText(FText::FromString(LifeStr));
        break;
    case 2:
        if (tPlayerName3) tPlayerName3->SetText(FText::FromString(Nickname));
        if (tPlayerFatigueRate3) tPlayerFatigueRate3->SetText(FText::FromString(FatigueStr));
        if (tPlayerLife3) tPlayerLife3->SetText(FText::FromString(LifeStr));
        break;
    case 3:
        if (tPlayerName4) tPlayerName4->SetText(FText::FromString(Nickname));
        if (tPlayerFatigueRate4) tPlayerFatigueRate4->SetText(FText::FromString(FatigueStr));
        if (tPlayerLife4) tPlayerLife4->SetText(FText::FromString(LifeStr));
        break;
    }
}