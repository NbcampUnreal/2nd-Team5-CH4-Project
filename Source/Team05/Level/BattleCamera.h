// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "BattleCamera.generated.h"

class ABaseCharacter;

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
    float YMultiplier = 1.2f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MinY = 800.f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MaxY = 1800.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MoveInterpSpeed = 0.3f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float UpdateInterval = 0.1f;

    UPROPERTY(VisibleAnywhere)
    TArray<ABaseCharacter*> TrackedActors;

    FTimerHandle UpdateTimerHandle;
};