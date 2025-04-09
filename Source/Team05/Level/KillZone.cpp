// Fill out your copyright notice in the Description page of Project Settings.


#include "KillZone.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AKillZone::AKillZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    PrimaryActorTick.bCanEverTick = true;

    // 루트 컴포넌트 설정
    DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    RootComponent = DefaultSceneRoot;

    // Arrow 컴포넌트 추가 및 루트에 붙이기
    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComponent->SetupAttachment(DefaultSceneRoot);

    Collision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collision"));
    Collision->SetupAttachment(DefaultSceneRoot);

    // 메시 설정 (Engine 기본 큐브 사용)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMesh.Succeeded())
    {
        Collision->SetStaticMesh(CubeMesh.Object);
    }

    // 콜리전 세팅 (필요에 따라)
    Collision->SetGenerateOverlapEvents(true);
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collision->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

    Collision->OnComponentBeginOverlap.AddDynamic(this, &AKillZone::OnCollisionBeginOverlap);
}

void AKillZone::OnCollisionBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
        UE_LOG(LogTemp, Warning, TEXT("KillZone 충돌: %s"), *OtherActor->GetName());

        FVector Location = OtherActor->GetActorLocation();
        FRotator Rotation = GetActorRotation(); 

        // 액터를 새로운 위치로 이동
        OtherActor->SetActorLocation(TeleportLocation);

        // 사운드 재생
        if (KillZoneSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, KillZoneSound, GetActorLocation());
        }

        // 파티클 생성
        if (P_Death)
        {
            UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAtLocation(
                GetWorld(),
                P_Death,
                FTransform(Rotation, Location, FVector(1.0f))  
            );

            if (PSC)
            {
                PSC->bAutoDestroy = true;
            }
        }


        //// 디버그 메시지
        //if (GEngine)
        //{
        //    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("KillZone!"));
        //}
	}
}

