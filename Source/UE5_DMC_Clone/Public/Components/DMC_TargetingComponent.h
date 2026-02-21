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
	
	void SoftLockOn();
	void RotateToTarget();
	void SnapToTarget(); // Instant rotation for combat snapping
	void StopRotation();
	void ClearSoftTarget();
	
	AActor* FindBestContextualTarget();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Rotation")
	TObjectPtr<UCurveFloat> RotationCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Targeting")
	float MaxSearchRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Targeting")
	float MaxFieldOfViewAngle = 60.f;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> FocusedTarget;
	
	FTimeline RotationTimeline;
	
	UFUNCTION()
	void HandleRotationTimelineProgress(float Value);
	
public:
	FORCEINLINE bool IsTargeting() const { return false; }
	FORCEINLINE TObjectPtr<AActor> GetTargetActor() const { return FocusedTarget; }
	FORCEINLINE TObjectPtr<AActor> GetSoftTarget() const { return FocusedTarget; }
};