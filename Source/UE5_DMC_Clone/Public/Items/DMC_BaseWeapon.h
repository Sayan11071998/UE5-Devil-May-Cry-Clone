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
	
	void Equip(
		TObjectPtr<USceneComponent> InParent,
		FName InSocketName,
		TObjectPtr<AActor> NewOwner,
		TObjectPtr<APawn> NewInstigator
	);
	
	void AttachMeshToSocket(TObjectPtr<USceneComponent> InParent, const FName& InSocketName);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	
public:
	// Weapon
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetWeaponMesh() const { return WeaponMesh; }
};