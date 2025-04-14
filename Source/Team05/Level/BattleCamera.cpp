#include "BattleCamera.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/BaseCharacter.h"
#include "TimerManager.h"

ABattleCamera::ABattleCamera()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleCamera::BeginPlay()
{
    Super::BeginPlay();

    // 타이머로 카메라 업데이트
    GetWorldTimerManager().SetTimerForNextTick([this]()
        {
            GetWorldTimerManager().SetTimer(UpdateTimerHandle, this, &ABattleCamera::UpdateCamera, 0.05f, true, 1.0f);
        });

}

void ABattleCamera::UpdateCamera()
{
    TrackedActors.RemoveAll([](AActor* Actor) { return !IsValid(Actor); });

    if (TrackedActors.Num() == 0)
    {
        // 다시 찾기 시도
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), FoundActors);

        for (AActor* Actor : FoundActors)
        {
            if (IsValid(Actor))
            {
                TrackedActors.Add(Actor);
            }
        }

        if (TrackedActors.Num() == 0)
        {
            return;
        }
    }

    FVector Min(FLT_MAX);
    FVector Max(-FLT_MAX);

    for (AActor* Actor : TrackedActors)
    {
        FVector Loc = Actor->GetActorLocation();
        Min.X = FMath::Min(Min.X, Loc.X);
        Min.Z = FMath::Min(Min.Z, Loc.Z);

        Max.X = FMath::Max(Max.X, Loc.X);
        Max.Z = FMath::Max(Max.Z, Loc.Z);
    }

    FVector Center;
    Center.X = (Min.X + Max.X) * 0.5f;
    Center.Y = GetActorLocation().Y;
    Center.Z = (Min.Z + Max.Z) * 0.5f;

    float DistanceXZ = FVector2D(Max.X - Min.X, Max.Z - Min.Z).Size();
    float TargetY = FMath::Clamp(DistanceXZ * YMultiplier, MinY, MaxY);

    FVector DesiredLocation = FVector(Center.X, TargetY, Center.Z+150);
    FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DesiredLocation, UpdateInterval, MoveInterpSpeed);

    SetActorLocation(NewLocation);

}
