#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Strafe.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UBTT_Strafe : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_Strafe();
	
protected:
	// ~ Begin UBTTaskNode interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// ~ End UBTTaskNode interface
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetKey;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float Radius = 400.f;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius = 10.f;
};