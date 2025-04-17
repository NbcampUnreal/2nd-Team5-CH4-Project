//GS_BattleState.cpp

#include "GS_BattleState.h"

#include "GameModes/Battle/PS_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameModes/Battle/GM_BattleMode.h"
#include "Character/MyPlayerController.h"
#include "TimerManager.h"
//debug
#include "Kismet/KismetSystemLibrary.h"

// 생성자 정의
AGS_BattleState::AGS_BattleState()
{
	// 복제 변수의 기본값 설정
	AlivePlayerControllerCount = 0;
	MatchState = EMatchState::Waiting;
}

void AGS_BattleState::Multicast_PrintMessage_Implementation(const FString& Msg)
{
	UKismetSystemLibrary::PrintString(
		this,
		Msg,
		true,   
		true,   
		FLinearColor::Yellow,
		10.0f
	);
}

void AGS_BattleState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 클라이언트와 동기화할 변수 지정
	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, MatchState);
}

// BeginPlay에서 UI 자동 갱신 타이머 시작
void AGS_BattleState::BeginPlay()
{
	Super::BeginPlay();
	StartBattleUIUpdate();
}

// 타이머 설정 함수
void AGS_BattleState::StartBattleUIUpdate()
{
	GetWorldTimerManager().SetTimer(
		TimerHandle_UpdateBattleUI,
		this,
		&AGS_BattleState::UpdateAllPlayerUI,
		0.5f,
		true
	);
}

// 각 플레이어 UI 갱신 처리
void AGS_BattleState::UpdateAllPlayerUI()
{
	for (APlayerState* PSBase : PlayerArray)
	{
		APS_PlayerState* PS = Cast<APS_PlayerState>(PSBase);
		if (!PS) continue;

		APawn* Pawn = PS->GetPawn();
		if (!Pawn) continue;

		APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
		if (!PC) continue;

		AMyPlayerController* MPC = Cast<AMyPlayerController>(PC);
		if (!MPC) continue;

		UMatchBattleWidget* Widget = MPC->GetMatchBattleWidget();
		if (!Widget) continue;

		Widget->UpdatePlayerStatus(PS->GetPlayerNum(), PS);
	}
}