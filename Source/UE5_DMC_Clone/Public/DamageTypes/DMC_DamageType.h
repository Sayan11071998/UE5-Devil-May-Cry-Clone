#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DMC_DamageType.generated.h"

// Enum to categorize the direction of the damage for hit reactions
UENUM(BlueprintType)
enum class EDMC_DamageType : uint8
{
	EDT_None       UMETA(DisplayName = "None"),
	EDT_Left       UMETA(DisplayName = "Left"),
	EDT_Right      UMETA(DisplayName = "Right"),
	EDT_Middle     UMETA(DisplayName = "Middle"),
	EDT_KnockBack  UMETA(DisplayName = "KnockBack")
};

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