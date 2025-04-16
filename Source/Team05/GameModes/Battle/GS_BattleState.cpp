//GS_BattleState.cpp

#include "GS_BattleState.h"

#include "GameModes/Battle/PS_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameModes/Battle/GM_BattleMode.h"
//debug
#include "Kismet/KismetSystemLibrary.h"


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
