#include "UI/DMC_EnemyHealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Enemies/DMC_EnemyCharacterBase.h"

void UDMC_EnemyHealthBar::SetHealthPercent(float InPercent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(InPercent);
	}
}

void UDMC_EnemyHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!OwnerActor) return;
	
	ADMC_EnemyCharacterBase* Enemy = Cast<ADMC_EnemyCharacterBase>(OwnerActor);
	if (!Enemy) return;

	if (Enemy->CanBeFinished())
	{
		if (HealthProgressBar)
		{
			HealthProgressBar->SetVisibility(ESlateVisibility::Collapsed);
		}
		
		if (ExecuteText)
		{
			ExecuteText->SetVisibility(ESlateVisibility::Visible);
			ExecuteText->SetColorAndOpacity(LowRangeColor);

			// Pulsing Animation
			PulseTime += InDeltaTime * PulseSpeed;
			float PulseValue = (FMath::Sin(PulseTime) + 1.0f) * 0.5f; // 0 to 1
			float Scale = FMath::Lerp(MinPulseScale, MaxPulseScale, PulseValue);
			ExecuteText->SetRenderScale(FVector2D(Scale));
		}
	}
	else
	{
		if (HealthProgressBar)
		{
			HealthProgressBar->SetVisibility(ESlateVisibility::Visible);
		}
		
		if (ExecuteText)
		{
			ExecuteText->SetVisibility(ESlateVisibility::Collapsed);
			PulseTime = 0.0f;
		}
	}
}