#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DMC_EnemyAIController.generated.h"

UCLASS()
class UE5_DMC_CLONE_API ADMC_EnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ADMC_EnemyAIController();
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetKeyName = FName("Target");
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName StrafeDistanceKeyName = FName("StrafeDistance");
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName AttackDistanceKeyName = FName("AttackDistance");
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName StateKeyName = FName("State");
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float StrafeDistance = 450.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AttackDistance = 175.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
};