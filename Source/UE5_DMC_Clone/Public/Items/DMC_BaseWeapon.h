#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMC_BaseWeapon.generated.h"

UCLASS()
class UE5_DMC_CLONE_API ADMC_BaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ADMC_BaseWeapon();
	virtual void Tick(float DeltaSeconds) override;
	
	void Equip(
		TObjectPtr<USceneComponent> InParent,
		FName InSocketName,
		TObjectPtr<AActor> NewOwner,
		TObjectPtr<APawn> NewInstigator
	);
	
	void AttachMeshToSocket(TObjectPtr<USceneComponent> InParent, const FName& InSocketName);
	
	// Called by the owner to enable/disable weapon collision
	void StartCollision(TSubclassOf<class UDMC_DamageType> DamageType);
	void EndCollision();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName StartSocketName = FName("Start");
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName EndSocketName = FName("End");
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TraceRadius = 20.f;
	
private:
	bool bIsCollisionActive = false;
	
	UPROPERTY()
	TSubclassOf<UDMC_DamageType> CurrentDamageType;
	
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;
	
	void HandleCollisionTracing();
	
public:
	// Weapon
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetWeaponMesh() const { return WeaponMesh; }
};