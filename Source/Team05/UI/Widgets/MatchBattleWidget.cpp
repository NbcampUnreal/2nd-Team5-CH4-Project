//MatchBattleWidget.cpp

#include "UI/Widgets/MatchBattleWidget.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/Battle/PS_PlayerState.h"

void UMatchBattleWidget::InitViewModelsFromGameState()
{
	const AGameStateBase* GS = GetWorld()->GetGameState();
	if (!IsValid(GS)) return;

	const TArray<APlayerState*>& PlayerArray = GS->PlayerArray;

	for (int32 i = 0; i < 4; ++i)
	{
		UPlayerStatusViewModel* VM = NewObject<UPlayerStatusViewModel>(this);

		if (i < PlayerArray.Num())
		{
			if (APS_PlayerState* PS = Cast<APS_PlayerState>(PlayerArray[i]))
			{
				VM->UpdateFromPlayerState(PS);
			}
		}
		else
		{
			VM->Life = 0;
			VM->FatigueRate = 0;
			VM->MatchHealth = 0;
			VM->Nickname = TEXT("Empty");
		}

		switch (i)
		{
		case 0: ViewModel1 = VM; break;
		case 1: ViewModel2 = VM; break;
		case 2: ViewModel3 = VM; break;
		case 3: ViewModel4 = VM; break;
		}
	}
}

void UMatchBattleWidget::BindViewModel(UPlayerStatusViewModel* InViewModel)
{
	// 단일 플레이어용 ViewModel 바인딩 (예: 로컬 플레이어)
	ViewModel1 = InViewModel; // 또는 ViewModel = InViewModel;
}