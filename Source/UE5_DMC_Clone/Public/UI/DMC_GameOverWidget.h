#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DMC_GameOverWidget.generated.h"

class UButton;

UCLASS()
class UE5_DMC_CLONE_API UDMC_GameOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()	
	void OnQuitClicked();
};