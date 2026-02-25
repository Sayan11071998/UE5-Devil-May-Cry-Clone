#include "Enemies/AI/Tasks/BTT_Attack.h"
#include "AIController.h"
#include "Enemies/DMC_EnemyCharacterBase.h"

UBTT_Attack::UBTT_Attack()
{
	NodeName = "Perform Attack";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		ADMC_EnemyCharacterBase* Enemy = Cast<ADMC_EnemyCharacterBase>(AIController->GetPawn());
		if (Enemy)
		{
			if (Enemy->PerformAttack(AttackIndex) > 0.f)
			{
				return EBTNodeResult::InProgress;
			}
		}
	}

	return EBTNodeResult::Failed;
}

void UBTT_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		ADMC_EnemyCharacterBase* Enemy = Cast<ADMC_EnemyCharacterBase>(AIController->GetPawn());
		if (Enemy && !Enemy->IsAttacking())
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}