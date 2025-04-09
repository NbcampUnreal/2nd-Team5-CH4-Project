// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PS_LobbyPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAM05_API APS_LobbyPlayer : public APlayerState
{
	GENERATED_BODY()
	
public:
	APS_LobbyPlayer();

	// 준비 여부 확인
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool IsReady() const;

	// 준비 여부 설정
	void SetReady(bool bReady);

	// 닉네임 설정 및 반환
	void SetNickname(const FString& InName);
	FString GetNickname() const;

	// 캐릭터 타입 설정 및 반환
	void SetCharacterType(const FName& InType);
	FName GetCharacterType() const;

protected:
	// Replicated 변수 설정을 위한 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// 캐릭터 선택 완료 여부
	UPROPERTY(Replicated)
	bool bIsReady = false;

	// 플레이어 닉네임
	UPROPERTY(Replicated)
	FString Nickname;

	// 선택한 캐릭터 이름 또는 타입
	UPROPERTY(Replicated)
	FName CharacterType;
};
