#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DMC_MainMenuWidget.generated.h"

class UButton;

UCLASS()
class UE5_DMC_CLONE_API UDMC_MainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	// ~ End UUserWidget Interface

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Menu")
	FName StartLevelName = FName("ThirdPersonMap");

	UFUNCTION()
	void OnStartClicked();

	UFUNCTION()
	void OnQuitClicked();
};