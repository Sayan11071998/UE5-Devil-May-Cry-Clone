#include "Enemies/AI/Tasks/BTT_Attack.h"
#include "AIController.h"
#include "Enemies/DMC_EnemyCharacterBase.h"

UBTT_Attack::UBTT_Attack()
{
	NodeName = "Perform Attack";
}

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		ADMC_EnemyCharacterBase* Enemy = Cast<ADMC_EnemyCharacterBase>(AIController->GetPawn());
		if (Enemy)
		{
			Enemy->PerformAttack();
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
