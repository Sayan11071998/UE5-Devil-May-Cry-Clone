#include "Enemies/DMC_EnemyRanged.h"
#include "Items/DMC_BaseProjectile.h"

ADMC_EnemyRanged::ADMC_EnemyRanged()
{
	AttackDistance = 800.f;
	StrafeDistance = 1200.f;
}

float ADMC_EnemyRanged::PerformAttack()
{
	return Super::PerformAttack();
}

void ADMC_EnemyRanged::Fire()
{
	if (!ProjectileClass || bDead) return;

	FVector SocketLocation = GetMesh()->GetSocketLocation(MuzzleSocketName);
	
	// Projectile travels in the enemy's forward direction
	FRotator FireRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	if (ADMC_BaseProjectile* Projectile = GetWorld()->SpawnActor<ADMC_BaseProjectile>(ProjectileClass, SocketLocation, FireRotation, SpawnParams))
	{
		// Projectile doesn't collide with the enemy that fired it
		Projectile->SetInstigator(this);
		if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(Projectile->GetRootComponent()))
		{
			RootPrim->IgnoreActorWhenMoving(this, true);
		}
	}
}