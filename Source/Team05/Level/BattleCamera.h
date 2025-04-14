// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "BattleCamera.generated.h"

UCLASS()
class TEAM05_API ABattleCamera : public ACameraActor
{
    GENERATED_BODY()

public:
    ABattleCamera();

protected:
    virtual void BeginPlay() override;

public:
    void UpdateCamera();

protected:
    UPROPERTY(EditAnywhere, Category = "Camera")
    float YMultiplier = 0.8f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MinY = 800.f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MaxY = 1800.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MoveInterpSpeed = 0.3f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float UpdateInterval = 0.5f;

    UPROPERTY()
    TArray<AActor*> TrackedActors;

    FTimerHandle UpdateTimerHandle;
};