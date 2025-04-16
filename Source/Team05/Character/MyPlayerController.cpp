//MyPlayerController.cpp

#include "MyPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ScriptMacros.h"
#include "UObject/UnrealType.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "GameModes/GI_BattleInstance.h"
#include "GameModes/Lobby/GM_LobbyMode.h"
#include "GameModes/Lobby/GS_LobbyState.h"
#include "GameModes/Battle/GS_BattleState.h"
#include "GameModes/Battle/PS_PlayerState.h"
#include "UI/Widgets/MatchResult.h"
#include "UI/Widgets/MatchBattleWidget.h"
#include "UI/Viewmodel/PlayerStatusViewModel.h"

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
			if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
			{
				//싱글모드인지 확인
				if (!GI->bSingleMode)
				{
					// 게임 플레이 여부 확인
					if (!GI->HasInitializedLobby())
					{
						if (NameInputUIClass)
						{
							NameInputUI = CreateWidget<UUserWidget>(this, NameInputUIClass);

							// UI 포인터 저장
							Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->NameInputUI = NameInputUI;

							if (NameInputUI)
							{
								NameInputUI->AddToViewport();
								GI->SetInitializedLobby(true); // 첫 진입 플래그 설정
								UE_LOG(LogTemp, Log, TEXT("NameInput UI created and shown."));
							}
						}
					}
				}
				else {
					if (APS_PlayerState* ps = Cast<APS_PlayerState>(this->PlayerState))
					{
						ps->SetPlayerNickName("Player");
						if (CharacterSelectUIClass)
						{
							CharacterSelectUI = CreateWidget<UUserWidget>(this, CharacterSelectUIClass);

							// UI 포인터 저장
							Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->CharacterSelectUI = CharacterSelectUI;

							if (CharacterSelectUI)
							{
								CharacterSelectUI->AddToViewport();
								GI->SetInitializedLobby(true); // 첫 진입 플래그 설정
								UE_LOG(LogTemp, Log, TEXT("Single Mode"));
							}
						}
					}
				}
			}
		}
		//// Battle 레벨에 진입 시 BattleWidget 연결
		//else if (CurrentLevelName.Contains(TEXT("Mario"))) // "Battle" 레벨이름 수정 필요
		//{
		//	InitializeBattleUIAndViewModels();
		//}
	}
}

//void AMyPlayerController::InitializeBattleUIAndViewModels()
//{
//	if (!MatchBattleUIClass) return;
//
//	// 1. UI 생성
//	MatchBattleUI = CreateWidget<UMatchBattleWidget>(this, MatchBattleUIClass);
//	if (!MatchBattleUI) return;
//
//	// 2. ViewModel 생성 및 연결
//	if (AGameStateBase* GameState = GetWorld()->GetGameState())
//	{
//		const TArray<APlayerState*>& PlayerArray = GameState->PlayerArray;
//
//		for (int32 i = 0; i < PlayerArray.Num(); ++i)
//		{
//			if (APS_PlayerState* PS = Cast<APS_PlayerState>(PlayerArray[i]))
//			{
//				// ViewModel 생성
//				UPlayerStatusViewModel* NewVM = NewObject<UPlayerStatusViewModel>(this);
//				NewVM->UpdateFromPlayerState(PS);
//
//				// 위젯에 연결
//				MatchBattleUI->SetViewModelAt(i, NewVM);
//			}
//		}
//	}
//
//	// 3. Add to viewport
//	MatchBattleUI->AddToViewport();
//
//	// 4. 입력 모드 전환
//	FInputModeGameAndUI InputMode;
//	InputMode.SetWidgetToFocus(MatchBattleUI->TakeWidget());
//	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
//	SetInputMode(InputMode);
//	SetShowMouseCursor(true);
//
//	UE_LOG(LogTemp, Log, TEXT("[Client] MatchBattleWidget UI created and initialized"));
//}

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
	}
}

void AMyPlayerController::Client_KickWithMessage_Implementation(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("[Kick] %s"), *Message);
	ClientTravel(TEXT("/Game/_Dev/Min/DevMenu"), ETravelType::TRAVEL_Absolute);
}

void AMyPlayerController::Server_SetReadyOnly_Implementation()
{
	if (APS_PlayerState* PS = GetPlayerState<APS_PlayerState>())
	{
		PS->SetReady(true);
		UE_LOG(LogTemp, Log, TEXT("[PlayerController] Ready 상태 설정 완료."));
	}
}

void AMyPlayerController::Client_SetInputEnabled_Implementation(bool bEnable)
{
	if (bEnable)
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
	else
	{
		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);
	}

	bShowMouseCursor = !bEnable;
}

FString AMyPlayerController::GetPlayerUniqueID() const
{
	if (APS_PlayerState* MyPS = GetPlayerState<APS_PlayerState>())
	{
		FString ID = MyPS->GetUniqueId().GetUniqueNetId().IsValid() ?
			GetPlayerState<APlayerState>()->GetUniqueId().GetUniqueNetId()->ToString() :
			FString("Invalid");
		return ID;
	}
	else {
		return "Error";
	}
}

void AMyPlayerController::Client_ShowMatchResultUI_Implementation()
{
	if (ResultUIClass && !ResultUI)
	{
		ResultUI = CreateWidget<UUserWidget>(this, ResultUIClass);
		if (ResultUI)
		{
			ResultUI->AddToViewport();

			// UI 전용 입력 모드로 변경
			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(ResultUI->GetCachedWidget());
			SetInputMode(Mode);
			SetShowMouseCursor(true);

			UE_LOG(LogTemp, Log, TEXT("[Client] MatchResult UI shown."));

			// GameInstance에서 순위 정보 가져와서 UI에 전달
			if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
			{
				if (UMatchResult* MatchResultWidget = Cast<UMatchResult>(ResultUI))
				{
					UE_LOG(LogTemp, Warning, TEXT("MatchResult 위젯 캐스트 성공"));
					for (int32 i = 0; i < GI->PlayerRanking.Num(); ++i)
					{
						const FString& PlayerID = GI->PlayerRanking[i];
						if (FPlayerInfo* Info = GI->PlayerInfoMap.Find(PlayerID))
						{
							FString RankText = FString::Printf(TEXT("%d위: %s"), i + 1, *Info->Nickname);
							MatchResultWidget->SetPlayerRankText(i + 1, RankText);
							UE_LOG(LogTemp, Log, TEXT("[Client] Rank %d - %s"), i + 1, *Info->Nickname);
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[Client] ResultUI cast to UMatchResult failed."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Client] BattleInstance cast failed."));
			}
		}
	}
}

void AMyPlayerController::Client_ReceiveRankingInfo_Implementation(const TArray<FPlayerRankingInfo>& RankingList)
{
	if (ResultUIClass && !ResultUI)
	{
		ResultUI = CreateWidget<UUserWidget>(this, ResultUIClass);
		if (ResultUI)
		{
			ResultUI->AddToViewport();
			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(ResultUI->GetCachedWidget());
			SetInputMode(Mode);
			SetShowMouseCursor(true);

			if (UMatchResult* ResultWidget = Cast<UMatchResult>(ResultUI))
			{
				for (const FPlayerRankingInfo& Info : RankingList)
				{
					ResultWidget->SetPlayerRankText(Info.Rank, Info.Nickname);
				}
			}
		}
	}
}

void AMyPlayerController::Server_CheckNickname_Implementation(const FString& Nickname)
{
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		if (APS_PlayerState* MyPS = GetPlayerState<APS_PlayerState>())
		{
			FString ID = MyPS->GetUniqueId().GetUniqueNetId().IsValid() ?
				GetPlayerState<APlayerState>()->GetUniqueId().GetUniqueNetId()->ToString() :
				FString("Invalid");
			// 닉네임 중복 확인
			bool bSuccess = GI->RegisterNickname(ID, Nickname);
			if (bSuccess)
			{
				MyPS->SetPlayerNickName(Nickname); // PlayerState에도 저장
				Client_OpenCharacterSelectWidget(); // 캐릭터 선택 UI 오픈
			}
			else
			{
				NameCheckText = FText::FromString(TEXT("중복된 닉네임입니다."));
				OnRep_NameCheckText();
			}
		}
	}
}

void AMyPlayerController::Server_SelectCharacter_Implementation(TSubclassOf<APawn> SelectedClass)
{
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		if (APS_PlayerState* MyPS = GetPlayerState<APS_PlayerState>())
		{
			FString ID = MyPS->GetUniqueId().GetUniqueNetId().IsValid() ?
				GetPlayerState<APlayerState>()->GetUniqueId().GetUniqueNetId()->ToString() :
				FString("Invalid");
			MyPS->SetCharacterClass(SelectedClass);

			GI->RegisterCharacterClass(ID, SelectedClass);

			// 캐릭터 선택 완료 후 서버에 알림
			Server_ConfirmSelection();
		}
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
	if (APS_PlayerState* PS = GetPlayerState<APS_PlayerState>())
	{
		//PS->SetReady(true);

		if (!GetPawn()) // 아직 Pawn 없으면 스폰
		{
			if (AGM_LobbyMode* GM = GetWorld()->GetAuthGameMode<AGM_LobbyMode>())
			{
				GM->SpawnPlayerInLobby(this);
			}
		}
	}
}

void AMyPlayerController::ReturnToTitle_Implementation()
{
	if (!HasAuthority())
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("DevMenu")), true);
	}
}

