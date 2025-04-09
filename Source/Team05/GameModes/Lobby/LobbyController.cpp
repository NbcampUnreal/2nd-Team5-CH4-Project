// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Lobby/LobbyController.h"

#include "Blueprint/UserWidget.h"
#include "GameModes/GI_BattleInstance.h"
#include "GameModes/Lobby/PS_LobbyPlayer.h"

void ALobbyController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALobbyController::PostNetInit()
{
	Super::PostNetInit();

	if (IsLocalPlayerController())
	{
		if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
		{
			if (GI->HasCompletedLobbySetup())
			{
				const FPlayerSelectInfo& Info = GI->GetSelectInfo();
				Server_SendSelectionInfo(Info.Nickname, Info.CharacterType);
			}
		}
	}
}

void ALobbyController::BeginPlay()
{
	Super::BeginPlay();

	// 클라이언트인 경우: GameInstance에서 선택 정보가 존재하면 서버로 전송
	if (IsLocalController())
	{
		if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
		{
			if (GI->HasCompletedLobbySetup())
			{
				const FPlayerSelectInfo& Info = GI->GetSelectInfo();
				Server_SendSelectionInfo(Info.Nickname, Info.CharacterType);
			}
		}

		UUserWidget* LobbyUI = CreateWidget<UUserWidget>(this, LobbyUIClass);
		if (IsValid(LobbyUI))
		{
			// 게임 전용 입력 모드 설정
			FInputModeGameOnly Mode;
			SetInputMode(Mode);
			bShowMouseCursor = false;

			LobbyUI->AddToViewport(1);
			LobbyUI->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ALobbyController::Server_SendSelectionInfo_Implementation(const FString& Nickname, const FName& CharacterType)
{
	// 서버에서 PlayerState에 선택 정보 저장
	if (APS_LobbyPlayer* PS = Cast<APS_LobbyPlayer>(PlayerState))
	{
		PS->SetNickname(Nickname);
		PS->SetCharacterType(CharacterType);
		PS->SetReady(true);
	}
}
