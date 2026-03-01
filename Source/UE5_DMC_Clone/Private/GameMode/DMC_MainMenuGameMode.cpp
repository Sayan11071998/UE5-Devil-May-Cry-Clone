#include "GameMode/DMC_MainMenuGameMode.h"
#include "Blueprint/UserWidget.h"

ADMC_MainMenuGameMode::ADMC_MainMenuGameMode()
{
}

void ADMC_MainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass)
	{
		UUserWidget* MainMenu = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
		if (MainMenu)
		{
			MainMenu->AddToViewport();

			// Set Input Mode to UI Only for the Main Menu
			if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
			{
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(MainMenu->TakeWidget());
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
			}
		}
	}
}