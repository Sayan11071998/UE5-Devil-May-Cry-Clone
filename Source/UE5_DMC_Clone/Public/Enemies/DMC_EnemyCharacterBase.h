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
	
	void Finished(AActor* PlayerAttacker);
	
	void SpawnHitFX(AActor* DamageCauser, const FHitResult& HitResult);
	
protected:
	virtual void Tick(float DeltaTime) override;
	
	// Link a Damage Type to a specific Animation and Pushback amount
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Hit Reaction")
	TMap<EDMC_DamageType, FDMC_HitReactionData> HitReactionMap;
	
	void PlayHitReaction(EDMC_DamageType DamageDirection);
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Health = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> FinishedMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDMC_CombatBufferComponent> BufferComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|VFX", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraSystem> HitVFX;
	
	bool bDead = false;
	
	void Death();
	
public:
	FORCEINLINE bool IsDead() const { return bDead; }
};