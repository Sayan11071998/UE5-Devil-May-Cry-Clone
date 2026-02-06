#pragma once

#include "CoreMinimal.h"
#include "EDMC_DamageType.h"
#include "GameFramework/DamageType.h"
#include "DMC_DamageType.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UDMC_DamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	EDMC_DamageType DamageType = EDMC_DamageType::EDT_None;
};