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
}

void APS_PlayerState::SetReady(bool bInReady)
{
	bReady = bInReady;
	//bReady = true;
}

bool APS_PlayerState::IsReady() const
{
	return bReady;
}

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

void APS_PlayerState::SetMatchHealth(int32 NewHealth)
{
	MatchHealth = NewHealth;
}

int32 APS_PlayerState::GetMatchHealth() const
{
	return MatchHealth;
}


void APS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS_PlayerState, PlayerNum);
	DOREPLIFETIME(APS_PlayerState, Nickname);
	DOREPLIFETIME(APS_PlayerState, CharacterClass);
	DOREPLIFETIME(APS_PlayerState, MatchHealth);
	DOREPLIFETIME(APS_PlayerState, bReady);
}

void APS_PlayerState::OnRep_Nickname()
{
	// 여기에서는 PlayerCharacter를 가져와서 UI를 업데이트하도록 지시할 수 있음

	APawn* OwnerPawn = GetPawn();
	if (IsValid(OwnerPawn))
	{
		class ABaseCharacter* BaseChar = Cast<ABaseCharacter>(OwnerPawn);
		if (IsValid(BaseChar))
		{
			BaseChar->UpdateNameTagUI(Nickname); 
		}
	}
}