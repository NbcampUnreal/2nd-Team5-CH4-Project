//MatchBattleWidget.cpp

#include "UI/Widgets/MatchBattleWidget.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/Battle/PS_PlayerState.h"

void UMatchBattleWidget::SetViewModelAt(int32 Index, UPlayerStatusViewModel* InViewModel)
{
	switch (Index)
	{
	case 0: ViewModel1 = InViewModel; break;
	case 1: ViewModel2 = InViewModel; break;
	case 2: ViewModel3 = InViewModel; break;
	case 3: ViewModel4 = InViewModel; break;
	default: break;
	}
}