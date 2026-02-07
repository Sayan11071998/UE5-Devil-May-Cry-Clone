#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DMC_EnemyCharacterBase.generated.h"

enum class EDMC_DamageType : uint8;

UCLASS()
class UE5_DMC_CLONE_API ADMC_EnemyCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADMC_EnemyCharacterBase();
	
	// ~ Begin APawn interface
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	// ~ End APawn interface
	
protected:
	virtual void Tick(float DeltaTime) override;
	
	// Hit Reaction Montages
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit Reaction")
	TObjectPtr<UAnimMontage> HitReactionLeft;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit Reaction")
	TObjectPtr<UAnimMontage> HitReactionRight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit Reaction")
	TObjectPtr<UAnimMontage> HitReactionMiddle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit Reaction")
	TObjectPtr<UAnimMontage> HitReactionKnockBack;
	
	// Buffer
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Buffer")
	TObjectPtr<UCurveFloat> BufferCurve;
	
	bool bIsBuffering = false;
	float CurrentBufferAmount = 0.f;
	float BufferTimeElapsed = 0.f;
	const float BufferDuration = 0.25f;
	
	// Hit Reaction
	void PlayHitReaction(EDMC_DamageType DamageDirection);
	
	// Buffer Play
	void StartBuffer(float Amount);
	void StopBuffer();
};