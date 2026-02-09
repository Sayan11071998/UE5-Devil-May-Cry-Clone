#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DamageTypes/DMC_DamageType.h"
#include "DMC_EnemyCharacterBase.generated.h"

USTRUCT(BlueprintType)
struct FDMC_HitReactionData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PushbackAmount = -6.f;
};

UCLASS()
class UE5_DMC_CLONE_API ADMC_EnemyCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADMC_EnemyCharacterBase();
	
	// ~ Begin APawn interface
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser	
	) override;
	// ~ End APawn interface
	
protected:
	virtual void Tick(float DeltaTime) override;
	
	// Link a Damage Type to a specific Animation and Pushback amount
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Hit Reaction")
	TMap<EDMC_DamageType, FDMC_HitReactionData> HitReactionMap;
	
	// Buffer
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffer")
	TObjectPtr<UCurveFloat> BufferCurve;
	
	bool bIsBuffering = false;
	float CurrentBufferAmount = 0.f;
	float BufferTimeElapsed = 0.f;
	const float BufferDuration = 0.25f;
	
	void UpdateBufferLogic(float DeltaTime);
	
	void PlayHitReaction(EDMC_DamageType DamageDirection);
	void StartBuffer(float Amount);
	void StopBuffer();
};