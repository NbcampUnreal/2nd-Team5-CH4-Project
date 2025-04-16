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

	FatigueRate = PlayerState->GetFatigueRate();
	Life = PlayerState->GetLife();
	MatchHealth = PlayerState->GetMatchHealth();
	Nickname = PlayerState->GetPlayerNickName();
}