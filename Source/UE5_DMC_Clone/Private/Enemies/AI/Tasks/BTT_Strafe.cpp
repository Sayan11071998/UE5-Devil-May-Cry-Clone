#include "Enemies/AI/Tasks/BTT_Strafe.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTT_Strafe::UBTT_Strafe()
{
	NodeName = "Strafe Move";
}

EBTNodeResult::Type UBTT_Strafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	if (AIController && BlackboardComponent && NavSystem)
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName));
		if (TargetActor)
		{
			FNavLocation RandomLocation;
			if (NavSystem->GetRandomReachablePointInRadius(TargetActor->GetActorLocation(), Radius, RandomLocation))
			{
				AIController->MoveToLocation(RandomLocation.Location, AcceptanceRadius);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	
	return EBTNodeResult::Failed;
}