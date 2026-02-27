#include "Items/DMC_BaseProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DamageTypes/DMC_DamageType.h"
#include "Particles/ParticleSystemComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"

ADMC_BaseProjectile::ADMC_BaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->InitSphereRadius(15.0f);
	SphereComp->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = SphereComp;

	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = SphereComp;
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 5.0f;
}

void ADMC_BaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComp->OnComponentHit.AddDynamic(this, &ADMC_BaseProjectile::OnProjectileHit);
}

void ADMC_BaseProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != GetOwner() && OtherActor != this)
	{
		if (ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(OtherActor))
		{
			UGameplayStatics::ApplyPointDamage(
				Player,
				Damage,
				Hit.ImpactNormal,
				Hit,
				GetInstigatorController(),
				GetOwner(),
				DamageTypeClass
			);
		}

		Destroy();
	}
}