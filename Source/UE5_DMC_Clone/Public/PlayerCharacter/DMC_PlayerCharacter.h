#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/DMC_ComboDataAsset.h"
#include "Interfaces/DMC_CombatInterface.h"
#include "DMC_CharacterTypes.h"
#include "DMC_PlayerCharacter.generated.h"

class UDMC_DamageType;
class ADMC_BaseWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UDMC_CombatBufferComponent;
class UDMC_TargetingComponent;

UCLASS()
class UE5_DMC_CLONE_API ADMC_PlayerCharacter : public ACharacter, public IDMC_CombatInterface
{
	GENERATED_BODY()

public:
	ADMC_PlayerCharacter();

	// Core State Management
	void SetState(EDMC_PlayerState NewState);
	void ResetDoubleJump();

	// Combat || Input Interface -> These are called by Input Bindings or Animation Notifies
	void LightAttack();
	void HeavyAttack();
	void Dodge();
	
	// Combat - Input Interface implementations
	virtual void SaveLightAttack() override;
	virtual void SaveHeavyAttack() override;
	virtual void SaveDodge() override;
	
	virtual void ResetState() override;
	
	// Targeting & Rotation implementations
	virtual void RotateToTarget() override;
	virtual void SetAllowPhysicsRotation(bool bAllow) override;
	virtual AActor* GetCombatTarget() const override;
	virtual AActor* GetSoftTarget() const override;
	
	// Weapon Collision implementations
	virtual void StartWeaponCollision(TSubclassOf<class UDMC_DamageType> DamageType) override;
	virtual void EndWeaponCollision() override;

	// Combat || Equipment & Collision
	void EquipWeapon();

	// Combat || Targeting
	void LockOn();
	void StopLockOn();
	void SoftLockOn();
	void StopRotation();

	// Damage Configuration
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DMC|Combat")
	TSubclassOf<UDMC_DamageType> DamageTypeClass;

protected:
	// Engine Overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	// ~ Begin ACharacter interface
	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;
	// ~ End ACharacter interface

	// Movement Handlers
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	FTimerHandle ChargeTimerHandle;

private:
	// Internal Implementation || Combat
	bool ExecuteAttack(UAnimMontage* Montage, float BufferAmount);
	void TryConsumeBufferedInput();

	bool PerformLightAttack(int32 InAttackIndex);
	bool PerformHeavyAttack(int32 InAttackIndex);
	bool PerformComboStarter();
	bool PerformComboExtender();
	void PerformChargeAttack();
	bool SpecialAttack();
	void PerformDodge();
	void FinisherAttack();
	
	void LightAttackReleased();
	void OnChargeTimerFinished();

	void ResetLightAttackVariables();
	void ResetHeavyAttackVariables();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DMC|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DMC|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDMC_CombatBufferComponent> BufferComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDMC_TargetingComponent> TargetingComp;

	// Input Action Config
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DMC|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DMC|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DMC|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DMC|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DMC|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DMC|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DMC|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DMC|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LockOnAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DMC|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> FinisherAttackAction;

	// Movement || Character Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DMC|Movement", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AActor>> CanLandClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DMC|Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DoubleJumpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DMC|Movement", meta = (AllowPrivateAccess = "true"))
	float DoubleJumpLaunchVelocity = 800.f;
	
	bool bDoubleJump = false;

	// Combat || State & Weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDMC_ComboDataAsset> ComboData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	EDMC_PlayerState CurrentState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ADMC_BaseWeapon> WeaponClass;
	
	UPROPERTY()
	TObjectPtr<ADMC_BaseWeapon> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DMC|Combat", meta = (AllowPrivateAccess = "true"))
	FName WeaponSocketName;

	int32 LightAttackIndex = 0;
	int32 HeavyAttackIndex = 0;
	int32 ComboExtenderIndex = 0;

	bool bSaveLightAttack = false;
	bool bSaveHeavyAttack = false;
	bool bDodgeAttackEnabled = false;
	bool bSaveDodge = false;
	bool bPerformChargeAttack = false;

public:
	// Specialized Getters
	FORCEINLINE TObjectPtr<USpringArmComponent> GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetDoubleJumpState() const { return bDoubleJump; }
	FORCEINLINE EDMC_PlayerState GetState() const { return CurrentState; }

	FORCEINLINE bool IsAttacking() const { return CurrentState == EDMC_PlayerState::ECS_Attack; }
	FORCEINLINE bool IsDodging() const { return CurrentState == EDMC_PlayerState::ECS_Dodge; }
	FORCEINLINE bool IsBusy() const { return IsAttacking() || IsDodging(); }
	FORCEINLINE bool IsStateEqualToAny(const TArray<EDMC_PlayerState>& StatesToCheck) const { return StatesToCheck.Contains(CurrentState); }

	// Targeting Getters
	bool GetIsTargeting() const;
	AActor* GetTargetActor() const { return GetCombatTarget(); }
};