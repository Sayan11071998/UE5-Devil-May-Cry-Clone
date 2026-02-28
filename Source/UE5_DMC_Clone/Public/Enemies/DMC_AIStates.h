#pragma once

#include "CoreMinimal.h"
#include "DMC_AIStates.generated.h"

UENUM(BlueprintType)
enum class EDMC_AIBehaviorState : uint8
{
	EAIS_None		UMETA(DisplayName = "None"),
	EAIS_Strafe		UMETA(DisplayName = "Strafe"),
	EAIS_Chase		UMETA(DisplayName = "Chase"),
	EAIS_Attack		UMETA(DisplayName = "Attack"),
	EAIS_Disabled	UMETA(DisplayName = "Disabled"),
	EAIS_Dead		UMETA(DisplayName = "Dead")
};