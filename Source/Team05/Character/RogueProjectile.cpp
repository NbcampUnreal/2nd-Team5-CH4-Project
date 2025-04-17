//MageProjectile.cpp


#include "RogueProjectile.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
ARogueProjectile::ARogueProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}
 
	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->InitSphereRadius(30.0f);
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 1500.0f;
		ProjectileMovementComponent->MaxSpeed = 1500.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = false;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if (!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>ArrowMesh(TEXT("/Game/_Blueprint/Character/Meshes/SM_Arrow.SM_Arrow"));
		if(ArrowMesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(ArrowMesh.Object);
		}

		static ConstructorHelpers::FObjectFinder<UMaterial>ArrowMaterial(TEXT("/Game/_Blueprint/Character/Meshes/M_Arrow.M_Arrow"));
		if (ArrowMaterial.Succeeded())
		{
			ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(ArrowMaterial.Object, ProjectileMeshComponent);
		}
		ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
		ProjectileMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
		ProjectileMeshComponent->SetupAttachment(RootComponent);
	}

	InitialLifeSpan = 2.0f;

	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	ProjectileMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

	// Replicate
	bReplicates = true;

	if (GetLocalRole() == ROLE_Authority)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARogueProjectile::OnProjectileImpact);
	}

	// 데미지 타입 초기화
	DamageType = UDamageType::StaticClass();
	Damage = 5.f;
}

// Called when the game starts or when spawned
void ARogueProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ARogueProjectile::OnProjectileImpact(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator())
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigator()->GetController(), this, DamageType);
		Destroy();
	}
}

// Called every frame
void ARogueProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARogueProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}
