#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DMC_ComboDataAsset.generated.h"

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
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Rage")
	TObjectPtr<UParticleSystem> RageParticles_1;
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Rage")
	TObjectPtr<UParticleSystem> RageParticles_2;
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Rage")
	TObjectPtr<UMaterialInterface> RageOverlayMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageTimeDilation = 1.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageDamageMultiplier = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rage")
	float RageDuration = 10.0f;
};