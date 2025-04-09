
#include "TitleGameMode.h"
#include "UI/UI_Controller/TitlePlayerController.h"

ATitleGameMode::ATitleGameMode()
{
    PlayerControllerClass = ATitlePlayerController::StaticClass();
    DefaultPawnClass = nullptr; // UI 전용이므로 Pawn 없음
}