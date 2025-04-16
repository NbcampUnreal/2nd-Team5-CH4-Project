//GS_BattleState.cpp

#include "GS_BattleState.h"

#include "GameModes/Battle/PS_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameModes/Battle/GM_BattleMode.h"
//debug
#include "Kismet/KismetSystemLibrary.h"

void AGS_BattleState::BeginPlay()
{
	Super::BeginPlay();

	// 서버 + 클라 모두 실행되도록
	if (MatchBattleWidgetClass && GetWorld()->IsGameWorld())
	{
		MatchBattleWidget = CreateWidget<UMatchBattleWidget>(GetWorld(), MatchBattleWidgetClass);

		if (MatchBattleWidget)
		{
			MatchBattleWidget->AddToViewport();

			// 로컬에서 ViewModel 초기화
			if (HasAuthority() == false)
			{
				const TArray<APlayerState*>& Players = PlayerArray;

				for (int32 i = 0; i < 4; ++i)
				{
					UPlayerStatusViewModel* VM = NewObject<UPlayerStatusViewModel>(this);

					if (i < Players.Num())
					{
						if (APS_PlayerState* PS = Cast<APS_PlayerState>(Players[i]))
						{
							VM->UpdateFromPlayerState(PS); // PS 정보 로컬에서 사용
						}
					}
					else
					{
						VM->Life = 0;
						VM->FatigueRate = 0;
						VM->MatchHealth = 0;
						VM->Nickname = TEXT("Empty");
					}

					MatchBattleWidget->SetViewModelAt(i, VM);
				}
			}
		}
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
