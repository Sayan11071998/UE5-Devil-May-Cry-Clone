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
	
	// ~ Begin IDMC_CombatInterface Implementation
	virtual bool CanBeFinished() const override;
	virtual void OnFinished(TObjectPtr<AActor> Attacker) override;
	virtual void SaveLightAttack() override {}
	virtual void SaveHeavyAttack() override {}
	virtual void SaveDodge() override {}
	virtual void ResetState() override {}
	virtual void RotateToTarget() override {}
	virtual void SetAllowPhysicsRotation(bool bAllow) override {}
	virtual TObjectPtr<AActor> GetCombatTarget() const override { return nullptr; }
	virtual TObjectPtr<AActor> GetSoftTarget() const override { return nullptr; }
	// ~ End IDMC_CombatInterface Implementation
	
	// ~ Begin AActor Interface
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	// ~ End AActor Interface
	
	// Visual feedback for hits
	void SpawnHitFX(TObjectPtr<AActor> DamageCauser, const FHitResult& HitResult);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// Map of damage types to their corresponding reactions
	UPROPERTY(EditDefaultsOnly, Category = "DMC|Combat")
	TMap<EDMC_DamageType, FDMC_HitReactionData> HitReactionMap;
	
	// Triggers a hit reaction animation and pushback
	void PlayHitReaction(EDMC_DamageType DamageDirection);
	
private:
	void Death(bool bIsFinisher = false);
	
	bool bDead = false;

	// Combat Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	float Health = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> FinishedMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDMC_CombatBufferComponent> BufferComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraSystem> HitVFX;

	// UI Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DMC|UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DMC|UI", meta = (AllowPrivateAccess = "true"))
	float HealthBarDrawSize_X = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DMC|UI", meta = (AllowPrivateAccess = "true"))
	float HealthBarDrawSize_Y = 50.f;
	
public:
	// Public state accessors
	FORCEINLINE bool IsDead() const { return bDead; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
};