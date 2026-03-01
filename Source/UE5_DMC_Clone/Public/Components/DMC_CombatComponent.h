#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DMC_CombatComponent.generated.h"

struct FDMC_AttackData;
class ADMC_PlayerCharacter;

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
	
	// Initiates attack combos
	void PerformLightAttack();
	void PerformHeavyAttack();
	
	// Initiates a dodge maneuver
	void PerformDodge();
	
	// Checks and executes available special attacks based on current context
	bool SpecialAttack();
	
	// Consumes any buffered input and executes the corresponding action
	void TryConsumeBufferedInput();
	
	// Attack execution with animation and buffering
	bool ExecuteAttack(const FDMC_AttackData& AttackData);

protected:
	virtual void BeginPlay() override;

private:
	bool Internal_ExecuteComboStep(const TArray<FDMC_AttackData>& ComboArray, int32& OutIndex);
	bool Internal_PerformComboStarter();
	bool Internal_PerformComboExtender();
	
	UPROPERTY()
	TObjectPtr<ADMC_PlayerCharacter> PlayerOwner;

	// Combo State
	FDMC_ComboState ComboState;

	// Combat flags
	bool bDodgeAttackEnabled = false;
	bool bPerformChargeAttack = false;
	
public:
	// Resets specific combo indices
	FORCEINLINE void ResetLightCombo() { ComboState.LightIndex = 0; }
	FORCEINLINE void ResetHeavyCombo() { ComboState.HeavyIndex = 0; }
	
	// Combat state getters
	FORCEINLINE int32 GetLightAttackIndex() const { return ComboState.LightIndex; }
	FORCEINLINE int32 GetHeavyAttackIndex() const { return ComboState.HeavyIndex; }
	FORCEINLINE int32 GetComboExtenderIndex() const { return ComboState.ExtenderIndex; }

	// Combat flag management
	FORCEINLINE void SetDodgeAttackEnabled(bool bEnabled) { bDodgeAttackEnabled = bEnabled; }
	FORCEINLINE void SetPerformChargeAttack(bool bPerform) { bPerformChargeAttack = bPerform; }
	FORCEINLINE bool GetDodgeAttackEnabled() const { return bDodgeAttackEnabled; }
	FORCEINLINE bool GetPerformChargeAttack() const { return bPerformChargeAttack; }
};