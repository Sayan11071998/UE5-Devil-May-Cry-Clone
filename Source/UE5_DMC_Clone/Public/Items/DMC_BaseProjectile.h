#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMC_BaseProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UDMC_DamageType;
class UParticleSystemComponent;

UCLASS()
class UE5_DMC_CLONE_API ADMC_BaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ADMC_BaseProjectile();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UParticleSystemComponent> ParticleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float Damage = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<UDMC_DamageType> DamageTypeClass;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	FORCEINLINE TObjectPtr<UProjectileMovementComponent> GetProjectileMovement() const { return ProjectileMovement; }
};