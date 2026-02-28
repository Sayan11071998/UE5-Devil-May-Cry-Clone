#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "PlayerCharacter/DMC_CharacterTypes.h"
#include "DMC_DamageType.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UDMC_DamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	UDMC_DamageType();
	
	// The category of this damage, used by the enemy's hit reaction system
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	EDMC_DamageType DamageType;
};