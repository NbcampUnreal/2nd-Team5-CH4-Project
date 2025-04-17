// RogueProjectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RogueProjectile.generated.h"

UCLASS()
class TEAM05_API  ARogueProjectile : public AActor
{
	GENERATED_BODY()

public:
	ARogueProjectile();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Category = "Projectile")
	void OnProjectileImpact(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//이 발사체가 가할 대미지 타입과 대미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageType> DamageType;
 
	//이 발사체가 가하는 대미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float Damage;
 
	
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
