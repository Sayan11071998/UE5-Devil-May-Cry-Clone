#pragma once

#include "CoreMinimal.h"
#include "DMC_CharacterTypes.generated.h"

UENUM(BlueprintType)
enum class EDMC_PlayerState : uint8
{
	ECS_Nothing UMETA(DisplayName = "Nothing"),
	ECS_Attack UMETA(DisplayName = "Attack"),
	ECS_Dodge UMETA(DisplayName = "Dodge")
};