//MageProjectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MageProjectile.generated.h"

UCLASS()
class TEAM05_API AMageProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMageProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 스피어 콜리전 컴포넌트
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComponent;

	// 발사체 이동 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

	// 발사체 메시 컴포넌트
	UPROPERTY(EditAnywhere, Category = Projectile)
	UStaticMeshComponent* ProjectileMeshComponent;

	// 발사체 머티리얼
	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	UMaterialInstanceDynamic* ProjectileMaterialInstance;

	// 발사체 속도 초기화 함수
	void FireInDirection(const FVector& ShootDirection);
};
