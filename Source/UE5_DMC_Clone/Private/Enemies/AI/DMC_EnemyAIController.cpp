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
		
		if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
		{
			APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			ADMC_EnemyCharacterBase* Enemy = Cast<ADMC_EnemyCharacterBase>(InPawn);
			
			BlackboardComponent->SetValueAsObject(TargetKeyName, Player);
			
			if (Enemy)
			{
				BlackboardComponent->SetValueAsFloat(StrafeDistanceKeyName, Enemy->GetStrafeDistance());
				BlackboardComponent->SetValueAsFloat(AttackDistanceKeyName, Enemy->GetAttackDistance());
			}
			else
			{
				BlackboardComponent->SetValueAsFloat(StrafeDistanceKeyName, StrafeDistance);
				BlackboardComponent->SetValueAsFloat(AttackDistanceKeyName, AttackDistance);
			}
		}
	}
}