// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ScriptMacros.h"
#include "UObject/UnrealType.h"
#include "Kismet/GameplayStatics.h"
#include "GameModes/GI_BattleInstance.h"
#include "GameModes/Lobby/GM_LobbyMode.h"
#include "GameModes/Lobby/PS_LobbyPlayer.h"

AMyPlayerController::AMyPlayerController()
	: InputMappingContext(nullptr),
	  MoveAction(nullptr),
	  JumpAction(nullptr),
	  DirectionAction(nullptr),
	  BaseAttackAction(nullptr),
	  SpecialAttackAction(nullptr),
	  SpecialMoveAction(nullptr),
	  GuardAction(nullptr),
	  EmoteAction(nullptr)
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	// 로비용 UI
	if (IsLocalController())
	{
		// 현재 레벨 이름 가져오기
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

		// "Lobby"라는 단어가 포함되어 있는지 검사
		if (CurrentLevelName.Contains(TEXT("Lobby")))
		{
			if (NameInputUIClass)
			{
				NameInputUI = CreateWidget<UUserWidget>(this, NameInputUIClass);

				// UI 포인터 저장
				Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->NameInputUI = NameInputUI;

				if (NameInputUI)
				{
					NameInputUI->AddToViewport();
					UE_LOG(LogTemp, Log, TEXT("NameInput UI created and shown."));
				}
			}
		}
	}
}

void AMyPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMyPlayerController::PostNetInit()
{
	Super::PostNetInit();
}

void AMyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AMyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerController, NameCheckText);
}

void AMyPlayerController::OnRep_NameCheckText()
{
	UE_LOG(LogTemp, Warning, TEXT("[OnRep] NameCheckText = %s"), *NameCheckText.ToString());

	if (NameInputUI)
	{
		FName FuncName = "UpdateWarningTextFromVar"; 
		if (NameInputUI->FindFunction(FuncName))
		{
			struct FWarningParam { FText Text; };
			FWarningParam Param{ NameCheckText };

			NameInputUI->ProcessEvent(NameInputUI->FindFunction(FuncName), &Param);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UI No Function"));
		}
	}
}

void AMyPlayerController::Server_CheckNickname_Implementation(const FString& Nickname)
{
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		bool bSuccess = GI->TryRegisterNickname(Nickname);

		if (APS_LobbyPlayer* MyPS = GetPlayerState<APS_LobbyPlayer>())
		{
			if (bSuccess)
			{
				MyPS->SetNickname(Nickname); // PlayerState에 저장
				Client_OpenCharacterSelectWidget(); // UI 전환
			}
			else
			{
				NameCheckText = FText::FromString(TEXT("중복된 닉네임입니다."));
				OnRep_NameCheckText();
			}
		}
	}
}

void AMyPlayerController::Server_SelectCharacter_Implementation(ECharacterType SelectedType)
{
	if (APS_LobbyPlayer* MyPS = GetPlayerState<APS_LobbyPlayer>())
	{
		MyPS->SetCharacterType(SelectedType);

		MyPS->RegisterToGameInstance();

		// 캐릭터 선택 완료 후 서버에 알림
		Server_ConfirmSelection();
	}
}

void AMyPlayerController::Client_OpenCharacterSelectWidget_Implementation()
{
	if (NameInputUI)
	{
		NameInputUI->RemoveFromParent();
		NameInputUI = nullptr;
	}

	if (CharacterSelectUIClass)
	{
		CharacterSelectUI = CreateWidget<UUserWidget>(this, CharacterSelectUIClass);
		if (CharacterSelectUI)
		{
			CharacterSelectUI->AddToViewport();

			UE_LOG(LogTemp, Log, TEXT("CharacterSelect UI Opened"));
		}
	}
}

void AMyPlayerController::Server_ConfirmSelection_Implementation()
{
	if (APS_LobbyPlayer* PS = GetPlayerState<APS_LobbyPlayer>())
	{
		PS->SetReady(true); // 준비 완료 처리

		if (!GetPawn()) // 아직 스폰 안 됐으면
		{
			if (AGM_LobbyMode* GM = GetWorld()->GetAuthGameMode<AGM_LobbyMode>())
			{
				GM->SpawnPlayerInLobby(this);
			}
		}
	}
}


