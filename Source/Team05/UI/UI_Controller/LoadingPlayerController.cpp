#include "LoadingPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/LoadingLayout.h"
#include "Kismet/GameplayStatics.h"

void ALoadingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (true == ::IsValid(UIWidgetClass))
	{
		UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
		if (true == ::IsValid(UIWidgetInstance))
		{
			UIWidgetInstance->AddToViewport();

			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = false;  // 로딩 화면에서는 커서 비활성화
		}
	}

	// 일정 시간 후 자동으로 레벨 이동
	GetWorldTimerManager().SetTimer(
		TransitionTimerHandle,
		this,
		&ALoadingPlayerController::GoToNextLevel,
		DelayBeforeTravel,
		false
	);
}

void ALoadingPlayerController::GoToNextLevel()
{
	UGameplayStatics::OpenLevel(this, NextLevelName);
}