// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class TEAM05_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputMappingContext* InputMappingContext;

	// 캐릭터 좌우 이동
	// vector1D
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* MoveAction;

	// 캐릭터 점프
	// bool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* JumpAction;

	//캐릭터 방향 벡터
	// vector 2D
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* DirectionAction;

	// 캐릭터 기본 공격
	// bool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* BaseAttackAction;

	// 캐릭터 특수 공격
	// bool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* SpecialAttackAction;

	// 캐릭터 특수 이동
	// bool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* SpecialMoveAction;
		
	// 캐릭터 방어
	// bool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* GuardAction;
	
	// 캐릭터 이모지
	// bool 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* EmoteAction;

	virtual void BeginPlay() override;

	// 서버 관련 함수들
	// 컴포넌트 초기화 이후 호출
	virtual void PostInitializeComponents() override;

	// 네트워크 초기화 이후 호출
	virtual void PostNetInit() override;

	// 캐릭터 소유권 획득 시 호출
	virtual void OnPossess(APawn* InPawn) override;

	// 복제 변수 등록
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
