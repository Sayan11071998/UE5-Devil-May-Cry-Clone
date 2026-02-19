#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCharacter/DMC_CharacterTypes.h"
#include "Data/DMC_ComboDataAsset.h"
#include "DMC_CombatComponent.generated.h"

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

	// ~ Begin Combat API
	/** Initiates a light attack combo step */
	void PerformLightAttack();
	
	/** Initiates a heavy attack combo step */
	void PerformHeavyAttack();
	
	/** Initiates a dodge maneuver */
	void PerformDodge();
	
	/** Checks and executes available special attacks based on current context */
	void SpecialAttack();
	
	/** Consumes any buffered input and executes the corresponding action */
	void TryConsumeBufferedInput();
	
	/** Low-level attack execution with animation and buffering */
	bool ExecuteAttack(const FDMC_AttackData& AttackData);
	// ~ End Combat API
	
	// ~ Begin State API
	/** Resets specific combo indices */
	void ResetLightCombo() { ComboState.LightIndex = 0; }
	void ResetHeavyCombo() { ComboState.HeavyIndex = 0; }
	
	/** Combat state getters */
	FORCEINLINE int32 GetLightAttackIndex() const { return ComboState.LightIndex; }
	FORCEINLINE int32 GetHeavyAttackIndex() const { return ComboState.HeavyIndex; }
	FORCEINLINE int32 GetComboExtenderIndex() const { return ComboState.ExtenderIndex; }

	/** Combat flag management */
	FORCEINLINE void SetDodgeAttackEnabled(bool bEnabled) { bDodgeAttackEnabled = bEnabled; }
	FORCEINLINE void SetPerformChargeAttack(bool bPerform) { bPerformChargeAttack = bPerform; }
	FORCEINLINE bool GetDodgeAttackEnabled() const { return bDodgeAttackEnabled; }
	FORCEINLINE bool GetPerformChargeAttack() const { return bPerformChargeAttack; }
	// ~ End State API

protected:
	virtual void BeginPlay() override;

private:
	// ~ Begin Internal Helpers
	/** Logic for iterating through a combo array safely */
	bool Internal_ExecuteComboStep(const TArray<FDMC_AttackData>& ComboArray, int32& OutIndex);
	
	/** Logic for high-to-low combo transitions */
	bool Internal_PerformComboStarter();
	
	/** Logic for low-to-high combo finishers */
	bool Internal_PerformComboExtender();
	// ~ End Internal Helpers
	
	/** The player character that owns this component */
	UPROPERTY()
	TObjectPtr<ADMC_PlayerCharacter> PlayerOwner;

	/** Internal tracking of combo indices */
	UPROPERTY(VisibleAnywhere, Category = "DMC|Combat")
	FDMC_ComboState ComboState;

	/** Transient combat flags */
	bool bDodgeAttackEnabled = false;
	bool bPerformChargeAttack = false;
};
