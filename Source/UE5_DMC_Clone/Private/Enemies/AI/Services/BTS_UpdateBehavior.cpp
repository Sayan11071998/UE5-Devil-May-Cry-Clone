#include "Enemies/AI/Services/BTS_UpdateBehavior.h"
#include "Enemies/DMC_AIStates.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"

UBTS_UpdateBehavior::UBTS_UpdateBehavior()
{
	NodeName = "Update Behavior State";
	bNotifyTick = true;
	Interval = 0.3f;
}

void UBTS_UpdateBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return;

	// Auto-initialize keys if names are not set in the editor
	if (TargetKey.SelectedKeyName.IsNone()) TargetKey.SelectedKeyName = FName("Target");
	if (StrafeDistanceKey.SelectedKeyName.IsNone()) StrafeDistanceKey.SelectedKeyName = FName("StrafeDistance");
	if (AttackDistanceKey.SelectedKeyName.IsNone()) AttackDistanceKey.SelectedKeyName = FName("AttackDistance");
	if (StateKey.SelectedKeyName.IsNone()) StateKey.SelectedKeyName = FName("State");

	APawn* Enemy = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (Enemy)
	{
		AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName));
		float StrafeDist = BlackboardComponent->GetValueAsFloat(StrafeDistanceKey.SelectedKeyName);
		float AttackDist = BlackboardComponent->GetValueAsFloat(AttackDistanceKey.SelectedKeyName);
		
		if (Target)
		{
			if (ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(Target))
			{
				if (Player->IsDead())
				{
					BlackboardComponent->SetValueAsEnum(StateKey.SelectedKeyName, static_cast<uint8>(EDMC_AIBehaviorState::EAIS_None));
					return;
				}
			}

			float Distance = Enemy->GetDistanceTo(Target);
			if (Distance <= AttackDist)
			{
				BlackboardComponent->SetValueAsEnum(StateKey.SelectedKeyName, static_cast<uint8>(EDMC_AIBehaviorState::EAIS_Attack));
			}
			else if (Distance <= StrafeDist)
			{
				BlackboardComponent->SetValueAsEnum(StateKey.SelectedKeyName, static_cast<uint8>(EDMC_AIBehaviorState::EAIS_Strafe));
			}
			else
			{
				BlackboardComponent->SetValueAsEnum(StateKey.SelectedKeyName, static_cast<uint8>(EDMC_AIBehaviorState::EAIS_Chase));
			}
		}
	}
}