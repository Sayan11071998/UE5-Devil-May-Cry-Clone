#pragma once

#include "CoreMinimal.h"
#include "Enemies/DMC_EnemyCharacterBase.h"
#include "DMC_EnemyRanged.generated.h"

class ADMC_BaseProjectile;

UCLASS()
class UE5_DMC_CLONE_API ADMC_EnemyRanged : public ADMC_EnemyCharacterBase
{
	GENERATED_BODY()

public:
	ADMC_EnemyRanged();

	virtual float PerformAttack() override;

	UFUNCTION(BlueprintCallable, Category = "DMC|Combat")
	void Fire();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "DMC|Combat")
	TSubclassOf<ADMC_BaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Combat")
	FName MuzzleSocketName = FName("MuzzleSocket");
};