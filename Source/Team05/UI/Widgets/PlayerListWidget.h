// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerListWidget.generated.h"

class UVerticalBox;
class UPlayerEntryWidget;

/**
 * 
 */
UCLASS()
class TEAM05_API UPlayerListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 닉네임과 레디 상태로 항목 추가
	UFUNCTION(BlueprintCallable)
	void AddPlayerEntry(const FString& Nickname, bool bIsReady);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerList();

	UFUNCTION(BlueprintCallable)
	void ClearPlayerEntries();

protected:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PlayerListBox;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPlayerEntryWidget> PlayerEntryWidgetClass;
};
