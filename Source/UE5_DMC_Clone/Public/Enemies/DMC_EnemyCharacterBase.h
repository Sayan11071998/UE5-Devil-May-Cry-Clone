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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit Reaction")
	TObjectPtr<UAnimMontage> HitReactionLeft;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit Reaction")
	TObjectPtr<UAnimMontage> HitReactionRight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit Reaction")
	TObjectPtr<UAnimMontage> HitReactionMiddle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit Reaction")
	TObjectPtr<UAnimMontage> HitReactionKnockBack;
	
	void PlayHitReaction(EDMC_DamageType DamageDirection);
};