//GS_BattleState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/Battle/GM_BattleMode.h"
#include "GS_BattleState.generated.h"


UCLASS()
class TEAM05_API AGS_BattleState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// 복제 대상 등록
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 디버깅용 메세지 뿌리기
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PrintMessage(const FString& Msg);


public:
	// 생존한 플레이어 컨트롤러 수
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 AlivePlayerControllerCount = 0;

	// 현재 매치 상태 (대기/플레이/종료 등)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EMatchState MatchState = EMatchState::Waiting;

	UFUNCTION()
	void OnRep_PlayerArray();
};