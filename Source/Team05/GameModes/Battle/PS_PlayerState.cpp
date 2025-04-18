﻿//PS_PlayerState.cpp

#include "PS_PlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "Character/BaseCharacter.h"
#include "Character/MyPlayerController.h"
#include "UI/Widgets/PlayerListWidget.h"
#include "GameModes/Battle/GS_BattleState.h"
#include "Net/UnrealNetwork.h"

APS_PlayerState::APS_PlayerState()
{
	bReplicates = true;
	PlayerNum = 0;
	Nickname = TEXT("Player");
	CharacterClass = nullptr;
	bReady = false;
	// plus
	MatchHealth = 100;
	FatigueRate = 0;
	Life = 3;
}

// ---Ready 상태 관리---

void APS_PlayerState::SetReady(bool bInReady)
{
	bReady = bInReady;
	if (HasAuthority())
	{
		OnRep_ReadyState(); // 서버에서 직접 호출할 수도 있음 (선택)
	}
}

bool APS_PlayerState::IsReady() const
{
	return bReady;
}

// ---Player 기본 정보---

void APS_PlayerState::SetPlayerNum(int32 InNum)
{
	PlayerNum = InNum;
}

int32 APS_PlayerState::GetPlayerNum() const
{
	return PlayerNum;
}

void APS_PlayerState::SetPlayerNickName(FString InNickname)
{
	Nickname = InNickname;

	// 서버에서도 즉시 처리할 필요가 있다면 여기에 OnRep_Nickname 호출 가능
	if (HasAuthority())
	{
		OnRep_Nickname(); // 서버에서 직접 호출할 수도 있음 (선택)
	}
}

FString APS_PlayerState::GetPlayerNickName() const
{
	return Nickname;
}

void APS_PlayerState::SetCharacterClass(TSubclassOf<APawn> InClass)
{
	CharacterClass = InClass;
}

TSubclassOf<APawn> APS_PlayerState::GetCharacterClass() const
{
	return CharacterClass;
}

// ---게임 상태 정보---

void APS_PlayerState::SetMatchHealth(int32 NewHealth)
{
	if (HasAuthority())
	{
		MatchHealth = NewHealth;
		OnRep_MatchHealth();
	}
}

int32 APS_PlayerState::GetMatchHealth() const
{
	return MatchHealth;
}

void APS_PlayerState::Multicast_UpdateReadyState_Implementation(bool bNewReady)
{
	// 실제 변수도 갱신
	bReady = bNewReady;

	// UI 위젯도 갱신
	OnRep_ReadyState();
}

void APS_PlayerState::Multicast_NotifyReadyChanged_Implementation(bool bNewReady)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AMyPlayerController* PC = Cast<AMyPlayerController>(It->Get()))
		{
			PC->Client_RefreshPlayerList();
		}
	}
}
void APS_PlayerState::SetLife(int32 NewLife)
{
	if (HasAuthority())
	{
		Life = NewLife;
		OnRep_Life();
	}
}

int32 APS_PlayerState::GetLife() const
{
	return Life;
}

void APS_PlayerState::SetFatigueRate(int32 NewRate)
{
	if (HasAuthority())
	{
		FatigueRate = NewRate;
		OnRep_FatigueRate();
	}
}

int32 APS_PlayerState::GetFatigueRate() const
{
	return FatigueRate;
}

// ---OnRep 함수 - ViewModel/UI 연동---

void APS_PlayerState::OnRep_Nickname()
{

	if (APawn* OwnerPawn = GetPawn())
	{
		if (ABaseCharacter* BaseChar = Cast<ABaseCharacter>(OwnerPawn))
		{
			BaseChar->UpdateNameTagUI(Nickname);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("OnRep_Nickname() 호출됨: Nick=%s"), *Nickname);

	if (CachedMatchBattleWidget)
	{
		CachedMatchBattleWidget->UpdatePlayerStatus(CachedIndex, this);
	}

	if (AGS_BattleState* GS = GetWorld()->GetGameState<AGS_BattleState>())
	{
		GS->NotifyAllWidgetsToRefresh(this);
	}
}

void APS_PlayerState::OnRep_ReadyState()
{
	if (GetWorld())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AMyPlayerController* PC = Cast<AMyPlayerController>(It->Get()))
			{
				PC->Client_RefreshPlayerList();
			}
		}
	}
}


void APS_PlayerState::OnRep_MatchHealth()
{
	UE_LOG(LogTemp, Warning, TEXT("[OnRep_MatchHealth] PlayerNum=%d, HP=%d"), PlayerNum, MatchHealth);

	if (CachedMatchBattleWidget)
	{
		CachedMatchBattleWidget->UpdatePlayerStatus(CachedIndex, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedMatchBattleWidget is null"));
	}
}

void APS_PlayerState::OnRep_Life()
{
	// 기존대로 자기 자신의 MatchBattleWidget만 업데이트하되
	// 아래처럼 로그 추가해서 확인
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Life() 호출됨: Nick=%s Life=%d"), *GetPlayerNickName(), Life);

	if (CachedMatchBattleWidget)
	{
		CachedMatchBattleWidget->UpdatePlayerStatus(CachedIndex, this);
	}

	//  GameState를 통해 전체 위젯에 UI 갱신 요청
	if (AGS_BattleState* GS = GetWorld()->GetGameState<AGS_BattleState>())
	{
		GS->NotifyAllWidgetsToRefresh(this);
	}
}

void APS_PlayerState::OnRep_FatigueRate()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_FatigueRate() 호출됨: Nick=%s Fatigue=%d"), *GetPlayerNickName(), FatigueRate);

	if (CachedMatchBattleWidget)
	{
		CachedMatchBattleWidget->UpdatePlayerStatus(CachedIndex, this);
	}

	if (AGS_BattleState* GS = GetWorld()->GetGameState<AGS_BattleState>())
	{
		GS->NotifyAllWidgetsToRefresh(this);
	}
}

// ---Replication 설정---

void APS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS_PlayerState, PlayerNum);
	DOREPLIFETIME(APS_PlayerState, Nickname);
	DOREPLIFETIME(APS_PlayerState, CharacterClass);
	DOREPLIFETIME(APS_PlayerState, MatchHealth);
	DOREPLIFETIME(APS_PlayerState, FatigueRate);
	DOREPLIFETIME(APS_PlayerState, Life);
	DOREPLIFETIME(APS_PlayerState, bReady);
}