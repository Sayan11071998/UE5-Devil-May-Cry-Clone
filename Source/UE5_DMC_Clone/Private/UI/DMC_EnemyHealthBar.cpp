#include "UI/DMC_EnemyHealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

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

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), OwnerActor->GetActorLocation());

	if (ExecuteText)
	{
		if (Distance < DisplayRange)
		{
			ExecuteText->SetColorAndOpacity(StandardColor);
		}
		else
		{
			ExecuteText->SetColorAndOpacity(LowRangeColor);
		}
	}
}
