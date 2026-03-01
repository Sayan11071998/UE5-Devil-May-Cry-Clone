#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMC_EnemySpawner.generated.h"

class UBoxComponent;

UCLASS()
class UE5_DMC_CLONE_API ADMC_EnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:
	ADMC_EnemySpawner();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	TObjectPtr<UBoxComponent> SpawnTrigger;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TMap<TSubclassOf<AActor>, int32> EnemiesToSpawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnRadius = 500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnInterval = 5.0f;
	
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult	
	);
	
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	
	void SpawnEnemies();
	
private:
	FTimerHandle SpawnTimerHandle;
};