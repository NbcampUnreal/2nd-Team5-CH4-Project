// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyController.generated.h"

/**
 * 
 */
UCLASS()
class TEAM05_API ALobbyController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void PostNetInit() override;
	virtual void PostInitializeComponents() override;

	// 클라이언트 → 서버: 선택 정보 전달 RPC
	UFUNCTION(Server, Reliable)
	void Server_SendSelectionInfo(const FString& Nickname, const FName& CharacterType);


public:
	// UI 클래스 지정(로비 UI만들면 하기)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyUIClass;
};

