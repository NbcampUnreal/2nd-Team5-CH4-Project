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

#pragma region UI

public:
	// 닉네임 입력 UI 클래스 (블루프린트에서 할당 가능)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> NameInputUIClass;

	// 생성된 위젯 인스턴스
	UPROPERTY()
	UUserWidget* NameInputUI;

	// Name 중복 결과 표시 텍스트
	UPROPERTY(ReplicatedUsing = OnRep_NameCheckText, BlueprintReadOnly, Category = "Nickname")
	FText NameCheckText;

	// 캐릭터 선택 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> CharacterSelectUIClass;

	// 생성된 캐릭터 선택 UI 인스턴스
	UPROPERTY()
	UUserWidget* CharacterSelectUI;

	// 게임 결과 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> ResultUIClass;

	// 게임 결과 UI 인스턴스
	UPROPERTY()
	UUserWidget* ResultUI;

	UFUNCTION()
	void OnRep_NameCheckText();

	// 닉네임 체크 및 서버에 정보 등록
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_CheckNickname(const FString& Nickname);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SelectCharacter(TSubclassOf<APawn> SelectedClass);

	// 캐릭터 UI 열기(클라이언트)
	UFUNCTION(Client, Reliable)
	void Client_OpenCharacterSelectWidget();

	// 서버 퇴장시키기(클라이언트)
	UFUNCTION(Client, Reliable)
	void Client_KickWithMessage(const FString& Message);

#pragma endregion

public:
	// 캐릭터 선택 완료 후 서버에 알림
	UFUNCTION(Server, Reliable)
	void Server_ConfirmSelection();

	// 타이틀 화면으로 복귀 (클라이언트 RPC)
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ReturnToTitle();

	// 유저 고유 ID 문자열을 반환하는 함수
	UFUNCTION(BlueprintCallable, Category = "Player")
	FString GetPlayerUniqueID() const;

	// 게임 종료 위젯 띄워주기
	UFUNCTION(Client, Reliable)
	void Client_ShowMatchResultUI();
	void Client_ShowMatchResultUI_Implementation();

};
