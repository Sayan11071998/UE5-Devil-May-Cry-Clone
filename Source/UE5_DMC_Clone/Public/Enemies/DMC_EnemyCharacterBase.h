#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DamageTypes/DMC_DamageType.h"
#include "Components/WidgetComponent.h"
#include "Interfaces/DMC_CombatInterface.h"
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
class UE5_DMC_CLONE_API ADMC_EnemyCharacterBase : public ACharacter, public IDMC_CombatInterface
{
	GENERATED_BODY()

public:
	ADMC_EnemyCharacterBase();
	
	// ~ Begin IDMC_CombatInterface
	virtual bool CanBeFinished() const override;
	virtual void OnFinished(AActor* Attacker) override;
	
	// Buffers (minimal implementations for interface compliance if needed, though they have defaults now)
	virtual void SaveLightAttack() override {}
	virtual void SaveHeavyAttack() override {}
	virtual void SaveDodge() override {}
	virtual void ResetState() override {}
	virtual void RotateToTarget() override {}
	virtual void SetAllowPhysicsRotation(bool bAllow) override {}
	virtual class AActor* GetCombatTarget() const override { return nullptr; }
	virtual class AActor* GetSoftTarget() const override { return nullptr; }
	// ~ End IDMC_CombatInterface
	
	// ~ Begin APawn interface
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser	
	) override;
	// ~ End APawn interface
	
	void SpawnHitFX(AActor* DamageCauser, const FHitResult& HitResult);
	
protected:
	virtual void BeginPlay() override;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarWidget;
	
	bool bDead = false;
	
	void Death();
	
public:
	FORCEINLINE bool IsDead() const { return bDead; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
};