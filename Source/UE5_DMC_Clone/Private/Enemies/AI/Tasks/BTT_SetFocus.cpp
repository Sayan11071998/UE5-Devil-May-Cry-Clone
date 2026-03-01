#include "Enemies/AI/Tasks/BTT_SetFocus.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_SetFocus::UBTT_SetFocus()
{
	NodeName = "Set Focus";
}

EBTNodeResult::Type UBTT_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	
	if (AIController && BlackboardComponent)
	{
		AActor* FocusTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName));
		if (FocusTarget)
		{
			AIController->SetFocus(FocusTarget);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}