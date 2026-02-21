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

protected:
	virtual void BeginPlay() override;

private:
	void ExecuteNextRageStage();
	void EnterRageMode();

	int32 CurrentStageIndex = 0;
	bool bRageActive = false;
	bool bIsRageSequencePlaying = false;

	FTimerHandle RageTimerHandle;
	FTimerHandle DurationTimerHandle;

	UPROPERTY()
	TArray<TObjectPtr<UParticleSystemComponent>> ActiveRageEmitters;
	
public:
	FORCEINLINE bool IsRageActive() const { return bRageActive; }
};