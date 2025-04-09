
#include "LoadingGameMode.h"
#include "UI/UI_Controller/LoadingPlayerController.h"

ALoadingGameMode::ALoadingGameMode()
{
    PlayerControllerClass = ALoadingPlayerController::StaticClass();
    DefaultPawnClass = nullptr;
}