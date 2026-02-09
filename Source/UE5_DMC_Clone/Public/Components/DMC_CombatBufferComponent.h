#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DMC_CombatBufferComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_DMC_CLONE_API UDMC_CombatBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDMC_CombatBufferComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Starts/Stops the movement lunge/knockback
	void StartBuffer(float Amount);
	void StopBuffer();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffer")
	TObjectPtr<UCurveFloat> BufferCurve;
	
private:
	bool bIsBuffering = false;
	float CurrentBufferAmount = 0.f;
	float BufferTimeElapsed = 0.f;
	const float BufferDuration = 0.25f;
};