#include "OneWayPlatform.h"
#include "Components/BoxComponent.h"
#include "components/capsulecomponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


AOneWayPlatform::AOneWayPlatform()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;

    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

    PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
    PlatformMesh->SetupAttachment(RootComponent);
}

void AOneWayPlatform::BeginPlay()
{
    Super::BeginPlay();

    PlatformZ = GetActorLocation().Z;

    // 틱함수말고 0.05초마다 CheckPlayersPosition() 실행
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckPlayer, this, &AOneWayPlatform::CheckPlayersPosition, 0.1f, true);
}




void AOneWayPlatform::CheckPlayersPosition()
{
    //모든 플레이어를 찾아 계산하는 방식 추후 변경가능
    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), FoundPlayers);

    for (AActor* Actor : FoundPlayers)
    {
        ACharacter* Player = Cast<ACharacter>(Actor);
        if (!Player) continue;

        // 발 위치 구하기
        float PlayerZ = Player->GetActorLocation().Z - Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();


        if (PlayerZ < PlatformZ)
        {
            // 아래에 있음 - 통과시킴
            CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
            CollisionBox->IgnoreActorWhenMoving(Player, true);
		}
        else
        {
            // 위에 있음 - 막음
            CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
            CollisionBox->IgnoreActorWhenMoving(Player, false);
        }
    }
}
