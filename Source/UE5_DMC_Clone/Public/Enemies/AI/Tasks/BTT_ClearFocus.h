#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ClearFocus.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UBTT_ClearFocus : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_ClearFocus();
	
protected:
	// ~ Begin UBTTaskNode interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// ~ End UBTTaskNode interface
};