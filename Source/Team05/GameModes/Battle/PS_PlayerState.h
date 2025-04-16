//PS_PlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UI/Viewmodel/PlayerStatusViewModel.h"
#include "PS_PlayerState.generated.h"

class UPlayerStatusViewModel;

UCLASS()
class TEAM05_API APS_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	APS_PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ---기본 플레이어 정보---
	
	// 플레이어 번호
	UPROPERTY(Replicated)
	int32 PlayerNum;

	UPROPERTY(ReplicatedUsing = OnRep_Nickname)
	FString Nickname;

	// 선택된 캐릭터 클래스
	UPROPERTY(Replicated)
	TSubclassOf<APawn> CharacterClass;

	// 준비 여부
	UPROPERTY(Replicated)
	bool bReady;

	// ---게임 상태 정보---
	
	UPROPERTY(ReplicatedUsing = OnRep_Life)
	int32 Life;

	// FatigueRate OnRep
	UPROPERTY(ReplicatedUsing = OnRep_FatigueRate)
	int32 FatigueRate;

	// MatchHealth OnRep
	UPROPERTY(ReplicatedUsing = OnRep_MatchHealth)
	int32 MatchHealth; //int32 MatchHealth = 100;
	
	// ---Getter / Setter---

	void SetPlayerNum(int32 InNum);
	int32 GetPlayerNum() const;

	void SetPlayerNickName(FString InNickname);
	FString GetPlayerNickName() const;

	void SetCharacterClass(TSubclassOf<APawn> InClass);
	TSubclassOf<APawn> GetCharacterClass() const;

	void SetReady(bool bInReady);
	bool IsReady() const;

	void SetLife(int32 InLife);
	int32 GetLife() const;

	void SetFatigueRate(int32 InRate);
	int32 GetFatigueRate() const;

	void SetMatchHealth(int32 InHealth);
	UFUNCTION(BlueprintCallable, Category = "Player")
	int32 GetMatchHealth() const;

	// ---ViewModel 연동---
	void SetViewModel(UPlayerStatusViewModel* InViewModel);

protected:
	UPROPERTY()
	TObjectPtr<UPlayerStatusViewModel> ViewModel; 

	// ---OnRep 함수들---

	UFUNCTION()
	void OnRep_Nickname();

	// UI Viewmodel
	UFUNCTION()
	void OnRep_FatigueRate();

	UFUNCTION()
	void OnRep_Life();

	UFUNCTION()
	void OnRep_MatchHealth();

};

