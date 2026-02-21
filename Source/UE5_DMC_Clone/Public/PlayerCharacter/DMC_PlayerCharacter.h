#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/DMC_CombatInterface.h"
#include "DMC_CharacterTypes.h"
#include "DMC_PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UDMC_ComboDataAsset;
class UDMC_CombatBufferComponent;
class UDMC_RageComponent;
class UDMC_TargetingComponent;
class UDMC_FinisherComponent;
class UDMC_CombatComponent;
class ADMC_BaseWeapon;
class UDMC_DamageType;

UCLASS()
class UE5_DMC_CLONE_API ADMC_PlayerCharacter : public ACharacter, public IDMC_CombatInterface
{
	GENERATED_BODY()

public:
	ADMC_PlayerCharacter();
	
	// Updates the current state of the player
	void SetState(EDMC_PlayerState NewState);
	
	// Resets the double jump flag
	void ResetDoubleJump();
	
	// Combat reset helpers
	void ResetLightAttackVariables();
	void ResetHeavyAttackVariables();
	
	// Attack inputs
	void LightAttack();
	void HeavyAttack();
	
	// Dodge input
	void Dodge();
	
	// Animation Notify Callbacks
	void SaveLightAttack();
	void SaveHeavyAttack();
	void SaveDodge();
	
	// State Check
	bool IsRaging() const;

	// ~ Begin IDMC_CombatInterface Implementation
	virtual void EnableHitStop(bool bInEnable) override { bHitStopEnabled = bInEnable; }
	virtual void HitStop() override;
	virtual void StartWeaponCollision(TSubclassOf<UDMC_DamageType> DamageType) override;
	virtual void EndWeaponCollision() override;
	
	virtual void RotateToTarget() override;
	virtual void SetAllowPhysicsRotation(bool bAllow) override;
	virtual TObjectPtr<AActor> GetCombatTarget() const override;
	virtual TObjectPtr<AActor> GetSoftTarget() const override;

	bool GetIsTargeting() const;
	// ~ End IDMC_CombatInterface Implementation

	// Combat Movement
	void StopRotation();
	void SoftLockOn();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;

	// Input Callbacks
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void LightAttackReleased();
	void FinisherAttack();
	void Rage();
	void StopRage();
	void OnChargeTimerFinished();

private:
	// Private Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDMC_CombatBufferComponent> BufferComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDMC_RageComponent> RageComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDMC_TargetingComponent> TargetingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDMC_FinisherComponent> FinisherComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDMC_CombatComponent> CombatComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Scene;

	// State Properties
	UPROPERTY(VisibleAnywhere, Category = "DMC|State")
	EDMC_PlayerState CurrentState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DMC|Movement", meta = (AllowPrivateAccess = "true"))
	float DoubleJumpLaunchVelocity = 400.0f;

	UPROPERTY(EditAnywhere, Category = "DMC|Movement")
	TArray<TSubclassOf<AActor>> CanLandClasses;

	bool bDoubleJump = false;

	// Combat Data
	UPROPERTY(EditDefaultsOnly, Category = "DMC|Combat")
	TObjectPtr<UDMC_ComboDataAsset> ComboData;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Combat")
	TSubclassOf<ADMC_BaseWeapon> WeaponClass;

	UPROPERTY()
	TObjectPtr<ADMC_BaseWeapon> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Combat")
	FName WeaponSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Combat")
	TObjectPtr<UAnimMontage> DoubleJumpMontage;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|HitStop")
	float HitStopTime = 0.05f;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|HitStop")
	float HitStopTimeDilation = 0.005f;

	FTimerHandle HitStopTimerHandle;
	bool bHitStopEnabled = false;

	// Input Assets
	UPROPERTY(EditDefaultsOnly, Category = "DMC|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Input")
	TObjectPtr<UInputAction> LightAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Input")
	TObjectPtr<UInputAction> HeavyAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Input")
	TObjectPtr<UInputAction> FinisherAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Input")
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Input")
	TObjectPtr<UInputAction> RageAction;

	UPROPERTY(EditDefaultsOnly, Category = "DMC|Input")
	TObjectPtr<UInputAction> StopRageAction;

	FTimerHandle ChargeTimerHandle;
	
	void EquipWeapon();
	void ResetState();
	
public:
	// State Checkers
	FORCEINLINE bool IsAttacking() const { return CurrentState == EDMC_PlayerState::ECS_Attack; }
	FORCEINLINE bool IsDodging() const { return CurrentState == EDMC_PlayerState::ECS_Dodge || CurrentState == EDMC_PlayerState::ECS_GeneralActions; }
	FORCEINLINE bool IsBusy() const { return IsAttacking() || IsDodging(); }
	FORCEINLINE bool IsStateEqualToAny(const TArray<EDMC_PlayerState>& StatesToCheck) const { return StatesToCheck.Contains(CurrentState); }

	// Getters & Setters
	FORCEINLINE TObjectPtr<UCameraComponent> GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetDoubleJumpState() const { return bDoubleJump; }
	FORCEINLINE EDMC_PlayerState GetState() const { return CurrentState; }

	FORCEINLINE TObjectPtr<UDMC_ComboDataAsset> GetComboData() const { return ComboData; }
	FORCEINLINE TObjectPtr<UDMC_RageComponent> GetRageComp() const { return RageComp; }
	FORCEINLINE TObjectPtr<UDMC_TargetingComponent> GetTargetingComp() const { return TargetingComp; }
	FORCEINLINE TObjectPtr<UDMC_CombatBufferComponent> GetBufferComponent() const { return BufferComponent; }
	FORCEINLINE TObjectPtr<UDMC_CombatComponent> GetCombatComp() const { return CombatComp; }
};