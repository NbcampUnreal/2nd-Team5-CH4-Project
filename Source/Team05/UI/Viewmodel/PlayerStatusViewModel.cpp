//PlayerStatusViewModel.cpp

#include "PlayerStatusViewModel.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/Battle/PS_PlayerState.h"

UPlayerStatusViewModel::UPlayerStatusViewModel()
{
	// 기본값 설정 (옵션)
	FatigueRate = 0;
	Life = 3;
	MatchHealth = 100;
	Nickname = TEXT("Unknown");
}

void UPlayerStatusViewModel::UpdateFromPlayerState(APS_PlayerState* PlayerState)
{
	if (!PlayerState) return;

	UE_MVVM_SET_PROPERTY_VALUE(FatigueRate, PlayerState->GetFatigueRate());
	UE_MVVM_SET_PROPERTY_VALUE(Life, PlayerState->GetLife());
	UE_MVVM_SET_PROPERTY_VALUE(MatchHealth, PlayerState->GetMatchHealth());
	UE_MVVM_SET_PROPERTY_VALUE(Nickname, PlayerState->GetPlayerNickName());
}