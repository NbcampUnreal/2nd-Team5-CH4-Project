// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawn.generated.h"

UCLASS()
class TEAM05_API ASpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawn();
#if WITH_EDITOR
    virtual void OnConstruction(const FTransform& Transform) override;
#endif

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UTextRenderComponent* TextRender;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UBillboardComponent* Billboard;

private:
    UPROPERTY(EditAnywhere, Category = "Spawn")
    int32 PlayerNum;
public:
	int32 GetPlayerNum() const { return PlayerNum; }
    FVector GetSpawnLocation() const { return GetActorLocation(); }
    FRotator GetSpawnRotation() const { return GetActorRotation(); }
};
