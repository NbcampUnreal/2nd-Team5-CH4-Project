
#include "TitlePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/TitleLayout.h"

void ATitlePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController())
    {
        if (true == ::IsValid(UIWidgetClass))
        {
            UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
            if (true == ::IsValid(UIWidgetInstance))
            {
                UIWidgetInstance->AddToViewport();
                UIWidgetInstance->SetIsFocusable(true);

                FInputModeUIOnly Mode;
                Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
                SetInputMode(Mode);

                bShowMouseCursor = true;
            }
        }
    }
}