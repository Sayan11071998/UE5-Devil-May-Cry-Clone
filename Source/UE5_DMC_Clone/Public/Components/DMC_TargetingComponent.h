#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "DMC_TargetingComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_DMC_CLONE_API UDMC_TargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDMC_TargetingComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void LockOn();
	void StopLockOn();
	void SoftLockOn();
	void RotateToTarget();
	void StopRotation();
	void ClearSoftTarget();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rotation")
	TObjectPtr<UCurveFloat> RotationCurve;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
	
	UPROPERTY()
	TObjectPtr<AActor> SoftTarget;
	
	FTimeline RotationTimeline;
	bool bIsTargeting = false;
	bool bInputHold = false;
	
	UFUNCTION()
	void HandleRotationTimelineProgress(float Value);
	
public:
	FORCEINLINE bool IsTargeting() const { return bIsTargeting; }
	FORCEINLINE TObjectPtr<AActor> GetTargetActor() const { return TargetActor; }
	FORCEINLINE TObjectPtr<AActor> GetSoftTarget() const { return SoftTarget; }
};