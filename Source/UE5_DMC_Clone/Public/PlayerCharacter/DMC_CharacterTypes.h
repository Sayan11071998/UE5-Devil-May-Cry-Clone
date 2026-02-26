#pragma once

#include "CoreMinimal.h"
#include "DMC_CharacterTypes.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class EDMC_PlayerState : uint8
{
	ECS_Nothing			UMETA(DisplayName = "Nothing"),
	ECS_Attack			UMETA(DisplayName = "Attack"),
	ECS_Dodge			UMETA(DisplayName = "Dodge"),
	ECS_Finisher		UMETA(DisplayName = "Finisher"),
	ECS_GeneralActions	UMETA(DisplayName = "GeneralActions")
};

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

USTRUCT(BlueprintType)
struct FDMC_HitReactionData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PushbackAmount = -6.f;
};