#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DMC_ComboDataAsset.generated.h"

UENUM(BlueprintType)
enum class EDMC_SpecialAttackRequirement : uint8
{
	ESAR_None UMETA(DisplayName = "None"),
	ESAR_RequiresTarget UMETA(DisplayName = "Requires Target"),
	ESAR_RequiresNoTarget UMETA(DisplayName = "Requires No Target"),
	ESAR_GroundOnly UMETA(DisplayName = "Ground Only"),
	ESAR_AirOnly UMETA(DisplayName = "Air Only")
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
	TArray<TObjectPtr<UAnimMontage>> LightAttackCombo;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combos|Light")
	TArray<TObjectPtr<UAnimMontage>> LightAttackRageCombo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combos|Heavy")
	TArray<TObjectPtr<UAnimMontage>> HeavyAttackCombo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combos|Special")
	TArray<TObjectPtr<UAnimMontage>> ComboStarterMontages;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combos|Special")
	TArray<TObjectPtr<UAnimMontage>> ComboExtenderMontages;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|SpecialAttacks")
	TObjectPtr<UAnimMontage> StingerAttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|SpecialAttacks")
	TObjectPtr<UAnimMontage> DodgeAttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|SpecialAttacks")
	TObjectPtr<UAnimMontage> FinisherAttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|SpecialAttacks")
	TObjectPtr<UAnimMontage> ChargeAttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Special")
	TObjectPtr<UAnimMontage> RageMontage;
	
	/** New Data-Driven Special Attacks */
	UPROPERTY(EditDefaultsOnly, Category = "Attack|SpecialAttacks")
	TArray<FDMC_SpecialAttackData> SpecialAttacks;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffers")
	float LightAttackBuffer = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffers")
	float HeavyAttackBuffer = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffers")
	float StarterAttackBuffer = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffers")
	float ExtenderAttackBuffer = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffers")
	float StingerAttackBuffer = 25.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffers")
	float ChargedBufferAmount = 5.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffers")
	float DodgeBufferAmount = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Dodge")
	TObjectPtr<UAnimMontage> DodgeMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Finisher")
	float FinisherAttackDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	TArray<FDMC_RageStage> RageSequence;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Rage", meta = (DEPRECATED = "Use RageSequence instead"))
	TObjectPtr<UParticleSystem> RageParticles_1;
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Rage", meta = (DEPRECATED = "Use RageSequence instead"))
	TObjectPtr<UParticleSystem> RageParticles_2;
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Rage", meta = (DEPRECATED = "Use RageSequence instead"))
	TObjectPtr<UMaterialInterface> RageOverlayMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageTimeDilation = 1.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageDamageMultiplier = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageDuration = 10.0f;
};