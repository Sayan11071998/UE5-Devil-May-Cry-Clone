#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DMC_PlayerHUD.generated.h"

class UProgressBar;

UCLASS()
class UE5_DMC_CLONE_API UDMC_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DMC|UI")
	void SetHealthPercent(float InPercent);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;
};