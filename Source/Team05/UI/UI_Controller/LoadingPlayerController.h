
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LoadingPlayerController.generated.h"

UCLASS()
class TEAM05_API ALoadingPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ALoadingPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> UIWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ALoadingPlayerController, Meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> UIWidgetInstance;

	// 자동 레벨 전환 관련
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ALoadingPlayerController, Meta = (AllowPrivateAccess))
	FName NextLevelName = TEXT("_MarioMap");  // 다음 레벨 이름

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ALoadingPlayerController, Meta = (AllowPrivateAccess))
	float DelayBeforeTravel = 3.0f;  // 전환까지 대기 시간 (초)

	FTimerHandle TransitionTimerHandle;

	void GoToNextLevel();
};
