// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

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
	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;

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
}

void AMyPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMyPlayerController::PostNetInit()
{
	Super::PostNetInit();

	// 클라이언트 확인 용도로 가장 적합한 위치
	if (IsLocalPlayerController())
	{
		UNetDriver* NetDriver = GetNetDriver();
		if (IsValid(NetDriver))
		{
			UNetConnection* ServerConnection = NetDriver->ServerConnection;
			// 서버 커넥션 확인 가능
		}
	}
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
