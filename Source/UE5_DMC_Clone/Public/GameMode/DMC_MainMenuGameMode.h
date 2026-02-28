#pragma once

#include "CoreMinimal.h"
#include "GameMode/DMC_GameModeBase.h"
#include "DMC_MainMenuGameMode.generated.h"

UCLASS()
class UE5_DMC_CLONE_API ADMC_MainMenuGameMode : public ADMC_GameModeBase
{
	GENERATED_BODY()

public:
	ADMC_MainMenuGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Menu")
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;
};
