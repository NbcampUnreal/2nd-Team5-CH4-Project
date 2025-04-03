// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_BattleState.h"

//debug
#include "Kismet/KismetSystemLibrary.h"

// 디버깅용 메세지 호출
void AGS_BattleState::Multicast_PrintMessage_Implementation(const FString& Msg)
{
	UKismetSystemLibrary::PrintString(
		this,
		Msg,
		true,   // 화면에 출력
		true,   // 로그에도 출력
		FLinearColor::Yellow,
		10.0f    // 지속 시간
	);
}
