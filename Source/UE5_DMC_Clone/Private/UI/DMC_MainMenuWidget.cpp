#include "UI/DMC_MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UDMC_MainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UDMC_MainMenuWidget::OnStartClicked);
	}
	
	if(QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UDMC_MainMenuWidget::OnQuitClicked);
	}
}

void UDMC_MainMenuWidget::OnStartClicked()
{
	if (!StartLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(GetWorld(), StartLevelName);
	}
}

void UDMC_MainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}