// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerEntryWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class TEAM05_API UPlayerEntryWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	// 닉네임과 준비 상태를 초기화하는 함수
	UFUNCTION(BlueprintCallable)
	void InitEntry(const FString& Nickname, bool bIsReady);


	void SetNickname(const FString& InName);
	void SetReadyStatus(bool bReady);


protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NicknameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ReadyText;
	
};
