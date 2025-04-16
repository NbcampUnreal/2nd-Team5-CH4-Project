//PS_PlayerState.cpp

#include "PS_PlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "Character/BaseCharacter.h"
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
	//bReady = true;
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
	if (HasAuthority())
	{
		Nickname = InNickname;
		OnRep_Nickname();
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

void APS_PlayerState::SetViewModel(UPlayerStatusViewModel* InViewModel)
{
	ViewModel = InViewModel;
}

// ---OnRep 함수 - ViewModel/UI 연동---

void APS_PlayerState::OnRep_Nickname()
{
	if (ViewModel)
	{
		ViewModel->Nickname = Nickname;
	}

	if (APawn* OwnerPawn = GetPawn())
	{
		if (ABaseCharacter* BaseChar = Cast<ABaseCharacter>(OwnerPawn))
		{
			BaseChar->UpdateNameTagUI(Nickname);
		}
	}
}

void APS_PlayerState::OnRep_MatchHealth()
{
	if (ViewModel)
	{
		ViewModel->MatchHealth = MatchHealth;
	}
}

void APS_PlayerState::OnRep_Life()
{
	if (ViewModel)
	{
		ViewModel->Life = Life;
	}
}

void APS_PlayerState::OnRep_FatigueRate()
{
	if (ViewModel)
	{
		ViewModel->FatigueRate = FatigueRate;
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