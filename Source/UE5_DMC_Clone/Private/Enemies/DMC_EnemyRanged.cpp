#include "Enemies/DMC_EnemyRanged.h"
#include "Items/DMC_BaseProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

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
	
	// Projectile travels in the enemy's forward direction
	FRotator FireRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADMC_BaseProjectile* Projectile = GetWorld()->SpawnActor<ADMC_BaseProjectile>(ProjectileClass, SocketLocation, FireRotation, SpawnParams);
	
	if (Projectile)
	{
		// Ensure the projectile doesn't collide with the enemy that fired it
		Projectile->SetInstigator(this);
		if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(Projectile->GetRootComponent()))
		{
			RootPrim->IgnoreActorWhenMoving(this, true);
		}
	}
}
