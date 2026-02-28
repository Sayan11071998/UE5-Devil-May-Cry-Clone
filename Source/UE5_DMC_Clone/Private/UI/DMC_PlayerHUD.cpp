#include "UI/DMC_PlayerHUD.h"
#include "Components/ProgressBar.h"

void UDMC_PlayerHUD::SetHealthPercent(float InPercent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(InPercent);
	}
}
