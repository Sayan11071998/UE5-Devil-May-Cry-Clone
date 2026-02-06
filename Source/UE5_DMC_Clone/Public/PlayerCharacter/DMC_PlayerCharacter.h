#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DMC_CharacterTypes.h"
#include "DMC_PlayerCharacter.generated.h"

class UDMC_DamageType;
class ADMC_BaseWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class UE5_DMC_CLONE_API ADMC_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADMC_PlayerCharacter();
	
	void SetState(EDMC_PlayerState NewState);
	void ResetDoubleJump();
	void ResetState();
	
	void ResetLightAttackVariables();
	void ResetHeavyAttackVariables();
	
	void SaveLightAttack();
	void SaveHeavyAttack();
	void SaveDodge();
	
	// Weapon Collision
	void StartWeaponCollision();
	void EndWeaponCollision();
	
	// Damage Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDMC_DamageType> DamageTypeClass;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	// Movement Methods
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;
	
	// Combat
	void EquipWeapon();
	
	// Light Attack Combo
	void LightAttack();
	bool PerformLightAttack(int32 InAttackIndex);
	
	// Heavy Attack Combo
	void HeavyAttack();
	bool PerformHeavyAttack(int32 InAttackIndex);
	
	// Heavy-Light Combo
	bool PerformComboStarter();
	bool PerformComboExtender();
	
	// Dodge
	void Dodge();
	void PerformDodge();
	
	// Buffer
	void StartBuffer(float Amount);
	void StopBuffer();

private:
	// Camera Settings
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LightAttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> HeavyAttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> DodgeAction;
	
	// Double Jump
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AActor>> CanLandClasses;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DoubleJumpMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	float DoubleJumpLaunchVelocity = 800.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	bool bDoubleJump = false;
	
	// Player State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EDMC_PlayerState CurrentState;
	
	// Player Weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ADMC_BaseWeapon>  WeaponClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ADMC_BaseWeapon> EquippedWeapon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FName WeaponSocketName;
	
	// Light Attack Combo
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Light Attack", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> LightAttackCombo;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Light Attack", meta = (AllowPrivateAccess = "true"))
	int32 LightAttackIndex = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Light Attack", meta = (AllowPrivateAccess = "true"))
	bool bSaveLightAttack = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Light Attack", meta = (AllowPrivateAccess = "true"))
	float LightAttackBufferAmount = 3.f;
	
	// Heavy Attack Combo
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Heavy Attack", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> HeavyAttackCombo;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Heavy Attack", meta = (AllowPrivateAccess = "true"))
	int32 HeavyAttackIndex = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Heavy Attack", meta = (AllowPrivateAccess = "true"))
	bool bSaveHeavyAttack = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Heavy Attack", meta = (AllowPrivateAccess = "true"))
	float HeavyAttackBufferAmount = 3.f;
	
	// Heavy-Light Attack Combo
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Combo", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> ComboStarterMontages;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Combo", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UAnimMontage>> ComboExtenderMontages;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Combo", meta = (AllowPrivateAccess = "true"))
	float StarterAttackBufferAmount = 3.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Combo", meta = (AllowPrivateAccess = "true"))
	float ExtenderAttackBufferAmount = 3.f;
	
	int32 ComboExtenderIndex = 0;
	
	// Dodge
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Dodge", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DodgeMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Dodge", meta = (AllowPrivateAccess = "true"))
	bool bSaveDodge = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Dodge", meta = (AllowPrivateAccess = "true"))
	float DodgeBufferAmount = 20.f;
	
	// Buffer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Buffer", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> BufferCurve;
	
	bool bIsBuffering = false;
	float CurrentBufferAmount = 0.f;
	float BufferTimeElapsed = 0.f;
	const float BufferDuration = 0.25f;
	
	// Weapon Collision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Weapon", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<AActor>> AlreadyHitActors;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Weapon", meta = (AllowPrivateAccess = "true"))
	bool bActiveCollision;
	
	// Enemy Class
	UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> EnemyClass;
	
public:
	// Components
	FORCEINLINE TObjectPtr<USpringArmComponent> GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetFollowCamera() const { return FollowCamera; }
	
	// Double Jump
	FORCEINLINE bool GetDoubleJumpState() const { return bDoubleJump; }
	
	// State
	FORCEINLINE EDMC_PlayerState GetState() const { return CurrentState; }
	FORCEINLINE bool IsStateEqualToAny(const TArray<EDMC_PlayerState>& StatesToCheck) const { return StatesToCheck.Contains(CurrentState); }
};