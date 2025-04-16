// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PlayerListWidget.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/Widgets/PlayerEntryWidget.h"
#include "GameModes/Battle/PS_PlayerState.h"


void UPlayerListWidget::AddPlayerEntry(const FString& Nickname, bool bIsReady)
{
	if (!PlayerEntryWidgetClass || !PlayerListBox) return;

	UPlayerEntryWidget* Entry = CreateWidget<UPlayerEntryWidget>(this, PlayerEntryWidgetClass);
	if (Entry)
	{
		Entry->SetNickname(Nickname);
		Entry->SetReadyStatus(bIsReady);
		PlayerListBox->AddChild(Entry);
	}
}

void UPlayerListWidget::UpdatePlayerList()
{
	if (PlayerListBox == nullptr) return;

	PlayerListBox->ClearChildren(); // 기존 목록 제거

	// 모든 PlayerController 순회
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			if (APS_PlayerState* PS = Cast<APS_PlayerState>(PC->PlayerState))
			{
				// 새로운 위젯 생성
				UPlayerEntryWidget* Entry = CreateWidget<UPlayerEntryWidget>(this, PlayerEntryWidgetClass);
				if (Entry)
				{
					Entry->InitEntry(PS->GetPlayerNickName(), PS->IsReady());
					PlayerListBox->AddChild(Entry);
				}
			}
		}
	}
}

void UPlayerListWidget::ClearPlayerEntries()
{
	// 예: VerticalBox 안의 항목들을 제거
	if (PlayerListBox)
	{
		PlayerListBox->ClearChildren();
	}
}
