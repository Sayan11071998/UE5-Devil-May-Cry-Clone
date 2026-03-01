#include "Enemies/DMC_EnemySpawner.h"
#include "NavigationSystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"

ADMC_EnemySpawner::ADMC_EnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SpawnTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnTrigger"));
	RootComponent = SpawnTrigger;
	SpawnTrigger->SetCollisionProfileName(TEXT("Trigger"));
}

void ADMC_EnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADMC_EnemySpawner::OnOverlapBegin);
}

void ADMC_EnemySpawner::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasSpawned) return;
	
	if (Cast<ADMC_PlayerCharacter>(OtherActor))
	{
		bHasSpawned = true;
		SpawnEnemies();
	}
}

void ADMC_EnemySpawner::SpawnEnemies()
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	if (!NavSystem) return;
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	for (auto& EnemyPair : EnemiesToSpawn)
	{
		TSubclassOf<AActor> EnemyClass = EnemyPair.Key;
		int32 Count = EnemyPair.Value;
		
		if (!EnemyClass) continue;
		
		for (int32 i = 0; i < Count; ++i)
		{
			FNavLocation RandomLocation;
			if (NavSystem->GetRandomReachablePointInRadius(GetActorLocation(), SpawnRadius, RandomLocation))
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				
				AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(EnemyClass, RandomLocation.Location, FRotator::ZeroRotator, SpawnParams);
				if (SpawnedEnemy && PlayerPawn)
				{
					FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(SpawnedEnemy->GetActorLocation(), PlayerPawn->GetActorLocation());
					SpawnedEnemy->SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f));
				}
			}
		}
	}
}