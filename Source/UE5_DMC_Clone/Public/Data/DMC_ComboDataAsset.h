#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DMC_ComboDataAsset.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UDMC_ComboDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Combos|Light")
	TArray<TObjectPtr<UAnimMontage>> LightAttackCombo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combos|Heavy")
	TArray<TObjectPtr<UAnimMontage>> HeavyAttackCombo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combos|Special")
	TArray<TObjectPtr<UAnimMontage>> ComboStarterMontages;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combos|Special")
	TArray<TObjectPtr<UAnimMontage>> ComboExtenderMontages;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combos|Launch")
	TObjectPtr<UAnimMontage> LaunchAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Buffers")
	float LightAttackBuffer = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Buffers")
	float HeavyAttackBuffer = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Buffers")
	float StarterAttackBuffer = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Buffers")
	float ExtenderAttackBuffer = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Buffers")
	float LaunchBuffer = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float DodgeBufferAmount = 20.f;
};