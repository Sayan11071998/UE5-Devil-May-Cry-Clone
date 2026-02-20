#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DMC_FinisherComponent.generated.h"

class ADMC_PlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE5_DMC_CLONE_API UDMC_FinisherComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDMC_FinisherComponent();

	// Tries to execute a finisher on the current target if conditions are met
	void TryExecuteFinisher();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<ADMC_PlayerCharacter> PlayerOwner;
};