// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Dev/MenuUI.h"

#include "GameModes/Menu/MenuController.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"


UMenuUI::UMenuUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMenuUI::NativeConstruct()
{
	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UMenuUI::OnPlayButtonClicked()
{
	AMenuController* PlayerController = GetOwningPlayer<AMenuController>();
	if (IsValid(PlayerController) == true)
	{
		FText ServerIP = ServerIPEditableText->GetText();
		PlayerController->JoinServer(ServerIP.ToString());
	}
}

void UMenuUI::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
