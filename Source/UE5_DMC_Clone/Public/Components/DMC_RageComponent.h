#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DMC_RageComponent.generated.h"

class UAnimMontage;
class UParticleSystem;
class UMaterialInterface;
class UParticleSystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_DMC_CLONE_API UDMC_RageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDMC_RageComponent();

	void StartRage();
	void StopRage();

	bool IsRageActive() const { return bRageActive; }

protected:
	virtual void BeginPlay() override;

private:
	void RageStage2();
	void RageStage3();
	void RageStage4();
	void EnterRageMode();

	bool bRageActive = false;

	FTimerHandle RageTimerHandle;
	FTimerHandle DurationTimerHandle;

	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> ActiveRageEmitter;
};
