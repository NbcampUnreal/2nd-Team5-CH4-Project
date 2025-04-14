//NameTagWidget.cpp

#include "UI/Widgets/NameTagWidget.h"
#include "Components/TextBlock.h"

void UNameTagWidget::SetNickname(const FString& InName)
{
	if (tNameTag)
	{
		tNameTag->SetText(FText::FromString(InName));
	}
}