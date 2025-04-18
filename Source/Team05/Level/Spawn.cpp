// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawn.h"
#include "Components/TextRenderComponent.h"
#include "Components/BillboardComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ASpawn::ASpawn()
{
    PrimaryActorTick.bCanEverTick = false;
    PlayerNum = 0;

    // Billboard
    Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	RootComponent = Billboard;

    // TextRender
    TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
    TextRender->SetupAttachment(RootComponent);
    TextRender->SetHorizontalAlignment(EHTA_Center);
    TextRender->SetWorldSize(80.0f);
    TextRender->SetHiddenInGame(true);
}

#if WITH_EDITOR
void ASpawn::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (TextRender)
    {
        FString Text = FString::Printf(TEXT("%d"), PlayerNum);
        TextRender->SetText(FText::FromString(Text));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Spawn] TextRender is null in OnConstruction (Server? %s)"), GIsServer ? TEXT("Yes") : TEXT("No"));
    }
}
#endif

