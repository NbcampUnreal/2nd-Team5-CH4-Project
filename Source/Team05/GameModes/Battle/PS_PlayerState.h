//PS_PlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PS_PlayerState.generated.h"


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
	UPROPERTY(ReplicatedUsing = OnRep_ReadyState)
	TSubclassOf<APawn> CharacterClass;

	// 준비 여부
	UPROPERTY(Replicated)
	bool bReady;

	// ---게임 상태 정보---
	
	UPROPERTY(ReplicatedUsing = OnRep_Life)
	int32 Life = 3;

	// FatigueRate OnRep
	UPROPERTY(ReplicatedUsing = OnRep_FatigueRate)
	int32 FatigueRate = 0;

	// MatchHealth OnRep
	UPROPERTY(ReplicatedUsing = OnRep_MatchHealth)
	int32 MatchHealth = 100;
	
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

	// 레디 상황 실시간 반영
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateReadyState(bool bNewReady);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_NotifyReadyChanged(bool bNewReady);
  

	// ---OnRep 함수들---
	// UI에 표시되는 값(FatigueRate, MatchHealth 등)은 반드시 Replicated + OnRep 로 설정

	UFUNCTION()
	void OnRep_Nickname();

	UFUNCTION()
	void OnRep_FatigueRate();

	UFUNCTION()
	void OnRep_Life();

	UFUNCTION()
	void OnRep_MatchHealth();

	UFUNCTION()
	void OnRep_ReadyState();

//각 플레이어의 상태가 바뀔 때
//클라이언트에 있는 UI를 자동으로 업데이트하려고 UI 위젯을 직접 기억(Caching)해두는 용도
	// 클라이언트 UI와 연결
	UPROPERTY()
	class UMatchBattleWidget* CachedMatchBattleWidget;

	UPROPERTY()
	int32 CachedIndex = -1; // 0~3
	//이 값들은 보통 MatchBattleWidget의 NativeConstruct() 시점에 설정
	// 
	//이 두 변수는 Replicated 아님 → 서버에 전달되지 않음

	//오직 클라이언트 내부에서만 사용

	//반드시 UI 생성 이후에 값 세팅해야 함(BeginPlay에서 곧바로 쓰면 nullptr일 수 있음)
};

