#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCharacter/DMC_CharacterTypes.h"
#include "DMC_CombatComponent.generated.h"

class ADMC_PlayerCharacter;
class UAnimMontage;
class UDMC_ComboDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE5_DMC_CLONE_API UDMC_CombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDMC_CombatComponent();

	// Combo Interface
	void PerformLightAttack();
	void PerformHeavyAttack();
	void TryConsumeBufferedInput();
	
	bool ExecuteAttack(UAnimMontage* Montage, float BufferAmount);
	
	// Index Resetters
	void ResetLightCombo();
	void ResetHeavyCombo();
	
	// Getters
	FORCEINLINE int32 GetLightAttackIndex() const { return LightAttackIndex; }
	FORCEINLINE int32 GetHeavyAttackIndex() const { return HeavyAttackIndex; }
	FORCEINLINE int32 GetComboExtenderIndex() const { return ComboExtenderIndex; }

protected:
	virtual void BeginPlay() override;

private:
	// Internal Helpers
	bool Internal_PerformLightAttack(int32 InAttackIndex);
	bool Internal_PerformHeavyAttack(int32 InAttackIndex);
	bool Internal_PerformComboStarter();
	bool Internal_PerformComboExtender();
	
	UPROPERTY()
	TObjectPtr<ADMC_PlayerCharacter> PlayerOwner;

	// Moved from Character
	int32 LightAttackIndex = 0;
	int32 HeavyAttackIndex = 0;
	int32 ComboExtenderIndex = 0;
};
