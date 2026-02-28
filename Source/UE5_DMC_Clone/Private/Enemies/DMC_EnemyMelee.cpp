#include "Enemies/DMC_EnemyMelee.h"
#include "Items/DMC_BaseWeapon.h"

ADMC_EnemyMelee::ADMC_EnemyMelee()
{
	AttackDistance = 175.f;
	StrafeDistance = 450.f;
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

void ADMC_EnemyMelee::Death(bool bIsFinisher)
{
	if (EquippedWeapon)
	{
		if (bIsFinisher)
		{
			EquippedWeapon->Destroy();
		}
		else
		{
			if (UStaticMeshComponent* WeaponMesh = EquippedWeapon->GetWeaponMesh())
			{
				WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			}
			EquippedWeapon->SetLifeSpan(5.0f);
		}
	}

	Super::Death(bIsFinisher);
}
