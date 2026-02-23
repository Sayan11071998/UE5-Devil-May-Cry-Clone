#include "Enemies/AI/Services/BTS_UpdateBehavior.h"
#include "Enemies/DMC_AIStates.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTS_UpdateBehavior::UBTS_UpdateBehavior()
{
	NodeName = "Update Behavior State";
	bNotifyTick = true;
	Interval = 0.3f;
}

void UBTS_UpdateBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	APawn* Enemy = OwnerComp.GetAIOwner()->GetPawn();
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (Enemy && BB)
	{
		AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));
		float StrafeDist = BB->GetValueAsFloat(StrafeDistanceKey.SelectedKeyName);
		
		if (Target)
		{
			float Distance = Enemy->GetDistanceTo(Target);
			if (Distance <= StrafeDist)
			{
				BB->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EDMC_AIBehaviorState::EAIS_Strafe);
			}
			else
			{
				BB->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EDMC_AIBehaviorState::EAIS_Chase);
			}
		}
	}
}