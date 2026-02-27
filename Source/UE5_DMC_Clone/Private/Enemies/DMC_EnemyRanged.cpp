#include "Enemies/DMC_EnemyRanged.h"
#include "Items/DMC_BaseProjectile.h"
#include "Kismet/KismetMathLibrary.h"

ADMC_EnemyRanged::ADMC_EnemyRanged()
{
	AttackDistance = 800.f;
	StrafeDistance = 1200.f;
}

float ADMC_EnemyRanged::PerformAttack()
{
	// Similar to base but ensures we have a projectile class? 
	// Or just use base implementation for montage playing and let Anim Notify call Fire()
	return Super::PerformAttack();
}

void ADMC_EnemyRanged::Fire()
{
	if (!ProjectileClass || bDead) return;

	FVector SocketLocation = GetMesh()->GetSocketLocation(MuzzleSocketName);
	FRotator SocketRotation = GetMesh()->GetSocketRotation(MuzzleSocketName);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	ADMC_BaseProjectile* Projectile = GetWorld()->SpawnActor<ADMC_BaseProjectile>(ProjectileClass, SocketLocation, SocketRotation, SpawnParams);
}
