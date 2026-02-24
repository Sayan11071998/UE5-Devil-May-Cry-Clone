#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_UpdateBehavior.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UBTS_UpdateBehavior : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTS_UpdateBehavior();
	
protected:
	// ~ Begin UBTService interface
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	// ~ End UBTService interface
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetKey;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector StrafeDistanceKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector AttackDistanceKey;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector StateKey;
};