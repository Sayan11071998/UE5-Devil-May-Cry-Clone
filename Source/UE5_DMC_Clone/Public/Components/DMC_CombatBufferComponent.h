#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DMC_CombatBufferComponent.generated.h"

UENUM(BlueprintType)
enum class EDMC_BufferedInput : uint8
{
	EBI_None		UMETA(DisplayName = "None"),
	EBI_LightAttack UMETA(DisplayName = "Light Attack"),
	EBI_HeavyAttack UMETA(DisplayName = "Heavy Attack"),
	EBI_Dodge		UMETA(DisplayName = "Dodge")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_DMC_CLONE_API UDMC_CombatBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDMC_CombatBufferComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Movement Buffer (Lunges/Knockbacks)
	void StartBuffer(float Amount);
	void StopBuffer();

	// Input Buffer (Buffered Commands)
	EDMC_BufferedInput PopInput();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffer")
	TObjectPtr<UCurveFloat> BufferCurve;
	
private:
	bool bIsBuffering = false;
	float CurrentBufferAmount = 0.f;
	float BufferTimeElapsed = 0.f;
	const float BufferDuration = 0.25f;

	EDMC_BufferedInput CurrentBufferedInput = EDMC_BufferedInput::EBI_None;
	
public:
	FORCEINLINE void BufferInput(EDMC_BufferedInput InputType) { CurrentBufferedInput = InputType; }
	FORCEINLINE void ClearInputBuffer() { CurrentBufferedInput = EDMC_BufferedInput::EBI_None; }
	FORCEINLINE bool HasBufferedInput() const { return CurrentBufferedInput != EDMC_BufferedInput::EBI_None; }
};