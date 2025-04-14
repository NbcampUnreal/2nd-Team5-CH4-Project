//NameTagWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NameTagWidget.generated.h"

class UTextBlock;

UCLASS()
class TEAM05_API UNameTagWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> tNameTag;

	UFUNCTION(BlueprintCallable)
	void SetNickname(const FString& InName);
};
