// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PlayerEntryWidget.h"

#include "Components/TextBlock.h"

void UPlayerEntryWidget::InitEntry(const FString& Nickname, bool bIsReady)
{
	if (NicknameText)
	{
		NicknameText->SetText(FText::FromString(Nickname));
	}

	if (ReadyText)
	{
		ReadyText->SetText(bIsReady ? FText::FromString(TEXT("Ready")) : FText::FromString(TEXT("Not Ready")));
	}
}

void UPlayerEntryWidget::SetNickname(const FString& InName)
{
	if (NicknameText)
		NicknameText->SetText(FText::FromString(InName));
}

void UPlayerEntryWidget::SetReadyStatus(bool bReady)
{
	if (ReadyText)
		ReadyText->SetText(FText::FromString(bReady ? TEXT("Ready") : TEXT("Not Ready")));
}

