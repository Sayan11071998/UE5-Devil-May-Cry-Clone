#include "Enemies/DMC_EnemyMelee.h"
#include "Items/DMC_BaseWeapon.h"

ADMC_EnemyMelee::ADMC_EnemyMelee()
{
}

void ADMC_EnemyMelee::BeginPlay()
{
	Super::BeginPlay();
	EquipWeapon();
}

void ADMC_EnemyMelee::EquipWeapon()
{
	if (!WeaponClass || EquippedWeapon) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	EquippedWeapon = GetWorld()->SpawnActor<ADMC_BaseWeapon>(WeaponClass, SpawnParams);
	if (EquippedWeapon)
	{
		EquippedWeapon->Equip(GetMesh(), WeaponSocketName, this, this);
	}
}

void ADMC_EnemyMelee::StartWeaponCollision(TSubclassOf<class UDMC_DamageType> DamageType)
{
	if (EquippedWeapon) EquippedWeapon->StartCollision(DamageType);
}

void ADMC_EnemyMelee::EndWeaponCollision()
{
	if (EquippedWeapon) EquippedWeapon->EndCollision();
}
