#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerCharacter/DMC_CharacterTypes.h"
#include "DMC_ComboDataAsset.generated.h"

UENUM(BlueprintType)
enum class EDMC_SpecialAttackRequirement : uint8
{
	ESAR_None				UMETA(DisplayName = "None"),
	ESAR_RequiresTarget		UMETA(DisplayName = "Requires Target"),
	ESAR_RequiresNoTarget	UMETA(DisplayName = "Requires No Target"),
	ESAR_RequiresModifier	UMETA(DisplayName = "Requires Modifier (Shift)"),
	ESAR_GroundOnly			UMETA(DisplayName = "Ground Only"),
	ESAR_AirOnly			UMETA(DisplayName = "Air Only"),
	ESAR_FinisherOnly		UMETA(DisplayName = "Finisher Only")
};

USTRUCT(BlueprintType)
struct FDMC_ParryData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ParryStartMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ParryEndMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ParrySuccessMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UNiagaraSystem> ParryFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundBase> ParrySound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PerfectParryWindow = 0.2f;
};

USTRUCT(BlueprintType)
struct FDMC_AttackData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BufferAmount = 3.f;
};

USTRUCT(BlueprintType)
struct FDMC_SpecialAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BufferAmount = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinForwardDot = -1.1f; // -1.1 means disabled

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxForwardDot = 1.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EDMC_SpecialAttackRequirement> Requirements;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCheckDodgeFlag = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCheckChargeFlag = false;
};

USTRUCT(BlueprintType)
struct FDMC_RageStage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UParticleSystem> StageFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> OverlayMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DelayToNextStage = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDestroyPreviousFX = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector FXScale = FVector(1.0f);
};

UCLASS()
class UE5_DMC_CLONE_API UDMC_ComboDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combos|Light")
	TArray<FDMC_AttackData> LightAttackCombo;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combos|Light")
	TArray<FDMC_AttackData> LightAttackRageCombo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combos|Heavy")
	TArray<FDMC_AttackData> HeavyAttackCombo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combos|Special")
	TArray<FDMC_AttackData> ComboStarterMontages;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combos|Special")
	TArray<FDMC_AttackData> ComboExtenderMontages;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Special")
	TObjectPtr<UAnimMontage> RageMontage;
	
	// Data-Driven Special Attacks
	UPROPERTY(EditDefaultsOnly, Category = "Attack|SpecialAttacks")
	TArray<FDMC_SpecialAttackData> SpecialAttacks;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Parry")
	FDMC_ParryData ParryData;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Dodge")
	FDMC_AttackData DodgeData;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Finisher")
	float FinisherAttackDistance = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	TArray<FDMC_RageStage> RageSequence;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageTimeDilation = 1.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageDamageMultiplier = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageDuration = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Player")
	float MaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Player")
	TMap<EDMC_DamageType, FDMC_HitReactionData> HitReactionMap;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Player")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Player")
	TObjectPtr<UParticleSystem> HitVFX;
};