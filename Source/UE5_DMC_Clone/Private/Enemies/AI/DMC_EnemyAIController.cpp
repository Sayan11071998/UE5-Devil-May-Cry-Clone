#include "Enemies/AI/DMC_EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

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
			BB->SetValueAsObject(TargetKeyName, Player);
		}
	}
}