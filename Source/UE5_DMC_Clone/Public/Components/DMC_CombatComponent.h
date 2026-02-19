#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCharacter/DMC_CharacterTypes.h"
#include "DMC_CombatComponent.generated.h"

class ADMC_PlayerCharacter;
class UAnimMontage;
class UDMC_ComboDataAsset;

USTRUCT(BlueprintType)
struct FDMC_ComboState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 LightIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 HeavyIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ExtenderIndex = 0;

	void Reset()
	{
		LightIndex = 0;
		HeavyIndex = 0;
		ExtenderIndex = 0;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE5_DMC_CLONE_API UDMC_CombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDMC_CombatComponent();

	void PerformLightAttack();
	void PerformHeavyAttack();
	void PerformDodge();
	void SpecialAttack();
	void TryConsumeBufferedInput();
	
	bool ExecuteAttack(UAnimMontage* Montage, float BufferAmount);
	
	// Index Resetters
	void ResetLightCombo() { ComboState.LightIndex = 0; }
	void ResetHeavyCombo() { ComboState.HeavyIndex = 0; }
	
	// Getters
	FORCEINLINE int32 GetLightAttackIndex() const { return ComboState.LightIndex; }
	FORCEINLINE int32 GetHeavyAttackIndex() const { return ComboState.HeavyIndex; }
	FORCEINLINE int32 GetComboExtenderIndex() const { return ComboState.ExtenderIndex; }

	// Flag State
	FORCEINLINE void SetDodgeAttackEnabled(bool bEnabled) { bDodgeAttackEnabled = bEnabled; }
	FORCEINLINE void SetPerformChargeAttack(bool bPerform) { bPerformChargeAttack = bPerform; }
	FORCEINLINE bool GetDodgeAttackEnabled() const { return bDodgeAttackEnabled; }
	FORCEINLINE bool GetPerformChargeAttack() const { return bPerformChargeAttack; }

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

	UPROPERTY(VisibleAnywhere, Category = "DMC|Combat")
	FDMC_ComboState ComboState;

	bool bDodgeAttackEnabled = false;
	bool bPerformChargeAttack = false;
};
