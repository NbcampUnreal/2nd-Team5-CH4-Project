// GS_BattleState.cpp


#include "GS_BattleState.h"

#include "Net/UnrealNetwork.h"
#include "GameModes/Battle/GM_BattleMode.h"
//debug
#include "Kismet/KismetSystemLibrary.h"

void AGS_BattleState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[GS_BattleState] BeginPlay: PlayerArray.Num() = %d"), PlayerArray.Num());

	for (int32 i = 0; i < PlayerArray.Num(); ++i)
	{
		APlayerState* PS = PlayerArray[i];
		if (PS)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Player[%d]: %s"), i, *PS->GetPlayerName());
		}
	}
}

void AGS_BattleState::OnRep_PlayerArray()
{
	UE_LOG(LogTemp, Warning, TEXT("[Client] OnRep_PlayerArray() 호출됨: %d명"), PlayerArray.Num());

	for (int32 i = 0; i < PlayerArray.Num(); ++i)
	{
		APlayerState* PS = PlayerArray[i];
		UE_LOG(LogTemp, Warning, TEXT("Client Player[%d] = %s"), i, *PS->GetPlayerName());
	}
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