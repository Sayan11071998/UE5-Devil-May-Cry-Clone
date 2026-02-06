#pragma once

#include "CoreMinimal.h"
#include "EDMC_DamageType.generated.h"

UENUM(BlueprintType)
enum class EDMC_DamageType : uint8
{
	EDT_None UMETA(DisplayName = "None"),
	EDT_Left UMETA(DisplayName = "Left"),
	EDT_Right UMETA(DisplayName = "Right"),
	EDT_Middle UMETA(DisplayName = "Middle"),
	EDT_KnockBack UMETA(DisplayName = "KnockBack")
};