// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Default.h"
#include "OneWayPlatform.generated.h"

class UBoxComponent;

UCLASS()
class TEAM05_API AOneWayPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOneWayPlatform();

    void BeginPlay();

private:
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* CollisionBox;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* PlatformMesh;

    float PlatformZ;

    FTimerHandle TimerHandle_CheckPlayer;

    void CheckPlayersPosition();
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UNavModifierComponent* NavModifier;
};
