#pragma once

#include "CoreMinimal.h"
#include "Enemies/DMC_EnemyCharacterBase.h"
#include "DMC_EnemyMelee.generated.h"

class ADMC_BaseWeapon;

UCLASS()
class UE5_DMC_CLONE_API ADMC_EnemyMelee : public ADMC_EnemyCharacterBase
{
	GENERATED_BODY()

public:
	ADMC_EnemyMelee();

	// ~ Begin IDMC_CombatInterface Implementation
	virtual void StartWeaponCollision(TSubclassOf<class UDMC_DamageType> DamageType) override;
	virtual void EndWeaponCollision() override;
	// ~ End IDMC_CombatInterface Implementation

protected:
	virtual void BeginPlay() override;
	virtual void Death(bool bIsFinisher = false) override;

private:
	void EquipWeapon();

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Combat")
	TSubclassOf<ADMC_BaseWeapon> WeaponClass;

	UPROPERTY()
	TObjectPtr<ADMC_BaseWeapon> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Combat")
	FName WeaponSocketName = FName("WeaponSocket");
};
