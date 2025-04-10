// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ScriptMacros.h"
#include "UObject/UnrealType.h"
#include "GameModes/GI_BattleInstance.h"

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

	// 게임 전용 입력 모드 설정
	/*FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;*/

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

	// 클라이언트에서만 실행 UI
	if (IsLocalController())
	{
		if (NameInputUIClass)
		{
			NameInputUI = CreateWidget<UUserWidget>(this, NameInputUIClass);

			if (NameInputUI)
			{
				NameInputUI->AddToViewport();

				// 입력 모드 변경: UIOnly로 전환 + 마우스 커서 표시
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(NameInputUI->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

				SetInputMode(InputMode);
				bShowMouseCursor = true;

				UE_LOG(LogTemp, Log, TEXT("NameInput UI created and shown."));
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
	// 클라이언트에서는 OnPossess가 호출되지 않음
	// 대신 OnRep_Owner()가 호출되어 소유자 초기화
}

void AMyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMyPlayerController::Server_CheckNickname_Implementation(const FString& Nickname)
{
	if (UGI_BattleInstance* GI = Cast<UGI_BattleInstance>(GetGameInstance()))
	{
		const bool bAvailable = !GI->IsNicknameTaken(Nickname);

		//닉네임 사용 가능하면 등록
		if (bAvailable)
		{
			// 캐릭터 선택 정보가 있다면 함께 등록 (예: PlayerState에서 가져오기)
			ECharacterType SelectedType = ECharacterType::Knight; // 기본 캐릭터
			GI->RegisterPlayerInfo(Nickname, SelectedType);
		}

		Client_NicknameCheckResult(bAvailable); // 결과 전달
	}
}

void AMyPlayerController::Client_NicknameCheckResult_Implementation(bool bAvailable)
{
	if (NameInputUI)
	{
		FName FunctionName = "UpdateWarningText";

		if (NameInputUI->FindFunction(FunctionName))
		{
			struct FNicknameParam { bool bAvailable; };
			FNicknameParam Param{ bAvailable };

			NameInputUI->ProcessEvent(NameInputUI->FindFunction(FunctionName), &Param);
		}
	}
}
