// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GS_BattleState.generated.h"

/**
 * 
 */
UCLASS()
class TEAM05_API AGS_BattleState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// 디버깅용 모든 클라이언트에 메시지를 띄우는 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PrintMessage(const FString& Msg);
};
