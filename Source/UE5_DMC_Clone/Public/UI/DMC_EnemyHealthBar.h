#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DMC_EnemyHealthBar.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class UE5_DMC_CLONE_API UDMC_EnemyHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthPercent(float InPercent);
	
protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// ~ End UUserWidget Interface
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ExecuteText;
	
	UPROPERTY(BlueprintReadOnly, Category = "Health Bar")
	TObjectPtr<AActor> OwnerActor;

	UPROPERTY()
	TObjectPtr<APawn> CachedPlayerPawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Bar")
	float DisplayRange = 1500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Bar")
	FLinearColor StandardColor = FLinearColor::White;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Bar")
	FLinearColor LowRangeColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Bar|Animation")
	float PulseSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Bar|Animation")
	float MinPulseScale = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Bar|Animation")
	float MaxPulseScale = 1.2f;

	float PulseTime = 0.0f;
	
public:
	FORCEINLINE void SetOwnerActor(TObjectPtr<AActor> InOwner) { OwnerActor = InOwner; }
	FORCEINLINE float GetDisplayRange() const { return DisplayRange; }
};