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

	// 플레이어 번호
	UPROPERTY(Replicated)
	int32 PlayerNum;

	// 기존 닉네임 선언을 수정
	UPROPERTY(ReplicatedUsing = OnRep_Nickname)
	FString Nickname;

	// 선택된 캐릭터 클래스
	UPROPERTY(ReplicatedUsing = OnRep_ReadyState)
	TSubclassOf<APawn> CharacterClass;

	// 게임 체력
	UPROPERTY(Replicated)
	int32 MatchHealth = 100;

	// 준비 여부
	UPROPERTY(Replicated)
	bool bReady;

	// Getter / Setter
	void SetReady(bool bInReady);
	bool IsReady() const;

	// Getter / Setter
	void SetPlayerNum(int32 InNum);
	int32 GetPlayerNum() const;

	void SetPlayerNickName(FString InNickname);
	FString GetPlayerNickName() const;

	void SetCharacterClass(TSubclassOf<APawn> InClass);
	TSubclassOf<APawn> GetCharacterClass() const;

	void SetMatchHealth(int32 NewHealth);
	UFUNCTION(BlueprintCallable, Category = "Player")
	int32 GetMatchHealth() const;

	// 레디 상황 실시간 반영
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateReadyState(bool bNewReady);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_NotifyReadyChanged(bool bNewReady);
  
//UI NameTag
protected:
	UFUNCTION()
	void OnRep_Nickname();

	UFUNCTION()
	void OnRep_ReadyState();

};
