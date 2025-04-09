
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitlePlayerController.generated.h"

class UTitleLayout;

UCLASS()
class TEAM05_API ATitlePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> UIWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> UIWidgetInstance;

};