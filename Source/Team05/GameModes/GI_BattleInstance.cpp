// Fill out your copyright notice in the Description page of Project Settings.


#include "GI_BattleInstance.h"

bool UGI_BattleInstance::HasCompletedLobbySetup() const
{
	return bHasCompletedLobbySetup;
}

void UGI_BattleInstance::SetCompletedLobbySetup(bool bCompleted)
{
	bHasCompletedLobbySetup = bCompleted;
}

const FPlayerSelectInfo& UGI_BattleInstance::GetSelectInfo() const
{
	return SelectInfo;
}

void UGI_BattleInstance::SetSelectInfo(const FPlayerSelectInfo& Info)
{
	SelectInfo = Info;
}

void UGI_BattleInstance::ResetSelection()
{
	bHasCompletedLobbySetup = false;
	SelectInfo = FPlayerSelectInfo();
}
