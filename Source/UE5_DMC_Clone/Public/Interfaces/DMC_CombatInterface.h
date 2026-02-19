#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DMC_CombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDMC_CombatInterface : public UInterface
{
	GENERATED_BODY()
};

class UE5_DMC_CLONE_API IDMC_CombatInterface
{
	GENERATED_BODY()

public:
	// Action Buffers
	virtual void SaveLightAttack() = 0;
	virtual void SaveHeavyAttack() = 0;
	virtual void SaveDodge() = 0;
	
	// State Management
	virtual void ResetState() = 0;
	
	// Targeting & Rotation
	virtual void RotateToTarget() = 0;
	virtual void SetAllowPhysicsRotation(bool bAllow) = 0;
	virtual class AActor* GetCombatTarget() const = 0;
	virtual class AActor* GetSoftTarget() const = 0;
	
	// Weapon Collision
	virtual void StartWeaponCollision(TSubclassOf<class UDMC_DamageType> DamageType) = 0;
	virtual void EndWeaponCollision() = 0;

	// Hit Stop
	virtual void EnableHitStop(bool bInEnable) = 0;
	virtual void HitStop() = 0;
};