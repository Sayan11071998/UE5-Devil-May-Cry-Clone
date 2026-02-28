#include "Enemies/AI/DMC_EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/DMC_EnemyCharacterBase.h"

ADMC_EnemyAIController::ADMC_EnemyAIController()
{
}

void ADMC_EnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		
		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			ADMC_EnemyCharacterBase* Enemy = Cast<ADMC_EnemyCharacterBase>(InPawn);
			
			BB->SetValueAsObject(TargetKeyName, Player);
			
			if (Enemy)
			{
				BB->SetValueAsFloat(StrafeDistanceKeyName, Enemy->GetStrafeDistance());
				BB->SetValueAsFloat(AttackDistanceKeyName, Enemy->GetAttackDistance());
			}
			else
			{
				BB->SetValueAsFloat(StrafeDistanceKeyName, StrafeDistance);
				BB->SetValueAsFloat(AttackDistanceKeyName, AttackDistance);
			}
		}
	}
}