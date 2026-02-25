#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_AttackChance.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UBTD_AttackChance : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTD_AttackChance();
	
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "0", ClampMax = "100"))
	int32 Chance;
	
protected:
	// ~ Begin UBTDecorator interface
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	// ~ End UBTDecorator interface
};