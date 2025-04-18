// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillZone.generated.h"

class UArrowComponent;
class UStaticMeshComponent;

UCLASS()
class TEAM05_API AKillZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKillZone();

	UFUNCTION()
	void OnCollisionBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* ArrowComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Collision;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	FVector TeleportLocation;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	FVector DeadLocation;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* KillZoneSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* P_Death;
};
