#include "PlayerCharacter/DMC_PlayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/DMC_CombatBufferComponent.h"
#include "Components/DMC_TargetingComponent.h"
#include "Enemies/DMC_EnemyCharacterBase.h"
#include "Items/DMC_BaseWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ADMC_PlayerCharacter::ADMC_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Initial Capsule Size
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	Scene->SetupAttachment(RootComponent);
	
	BufferComponent = CreateDefaultSubobject<UDMC_CombatBufferComponent>(TEXT("CombatBuffer"));
	TargetingComp = CreateDefaultSubobject<UDMC_TargetingComponent>(TEXT("TargetingComponent"));
	
	CurrentState = EDMC_PlayerState::ECS_Nothing;
}

void ADMC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	EquipWeapon();
}

void ADMC_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (ActiveRageEmitter)
	{
		ActiveRageEmitter->SetWorldLocation(Scene->GetComponentLocation());
	}
}

void ADMC_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADMC_PlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADMC_PlayerCharacter::Look);
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::LightAttack);
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Completed, this, &ADMC_PlayerCharacter::LightAttackReleased);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::HeavyAttack);
		EnhancedInputComponent->BindAction(FinisherAttackAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::FinisherAttack);
		
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::Dodge);
	
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::LockOn);
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Completed, this, &ADMC_PlayerCharacter::StopLockOn);
		
		EnhancedInputComponent->BindAction(RageAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::Rage);
		EnhancedInputComponent->BindAction(StopRageAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::StopRage);
	}
}

void ADMC_PlayerCharacter::SetState(EDMC_PlayerState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
	}
}

void ADMC_PlayerCharacter::ResetDoubleJump()
{
	bDoubleJump = false;
}

void ADMC_PlayerCharacter::LightAttack()
{
	bSaveDodge = false;
	ComboExtenderIndex = 0;
	bSaveHeavyAttack = false;

	if (CurrentState == EDMC_PlayerState::ECS_Dodge)
	{
		bDodgeAttackEnabled = true;
	}

	if (IsBusy()) 
	{
		bSaveLightAttack = true;
	}
	else
	{
		bHitStopEnabled = false;
		if (SpecialAttack()) return;

		if (!GetCharacterMovement()->IsFalling())
		{
			ResetHeavyAttackVariables();
			PerformLightAttack(LightAttackIndex);
		}
	}
	
	GetWorldTimerManager().SetTimer(ChargeTimerHandle, this, &ADMC_PlayerCharacter::OnChargeTimerFinished, 0.5f, false);
}

void ADMC_PlayerCharacter::HeavyAttack()
{
	bSaveLightAttack = false;
	bSaveDodge = false;
	
	if (IsBusy())
	{
		bSaveHeavyAttack = true;
	}
	else
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			ResetLightAttackVariables();
			PerformHeavyAttack(HeavyAttackIndex);
		}
	}
}

void ADMC_PlayerCharacter::Dodge()
{
	if (IsDodging() || CurrentState == EDMC_PlayerState::ECS_Finisher)
	{
		bSaveDodge = true;
	}
	else
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			PerformDodge();
		}
	}
}

void ADMC_PlayerCharacter::Jump()
{
	if (IsBusy() || CurrentState == EDMC_PlayerState::ECS_Finisher) return;
	
	if (GetCharacterMovement()->IsFalling())
	{
		if (!bDoubleJump)
		{
			bDoubleJump = true;
			
			if (DoubleJumpMontage)
			{
				PlayAnimMontage(DoubleJumpMontage);
			}
			
			FVector LaunchVelocity = FVector(0.f, 0.f, DoubleJumpLaunchVelocity);
			LaunchCharacter(LaunchVelocity, false, true);
		}
	}
	else
	{
		Super::Jump();
	}
}

void ADMC_PlayerCharacter::SaveLightAttack()
{
	TryConsumeBufferedInput();
}

void ADMC_PlayerCharacter::SaveHeavyAttack()
{
	TryConsumeBufferedInput();
}

void ADMC_PlayerCharacter::SaveDodge()
{
	TryConsumeBufferedInput();
}

void ADMC_PlayerCharacter::ResetState()
{
	SetState(EDMC_PlayerState::ECS_Nothing);
	ResetLightAttackVariables();
	ResetHeavyAttackVariables();
	
	bSaveDodge = false;
	BufferComponent->StopBuffer();
	ComboExtenderIndex = 0;
	
	TargetingComp->StopRotation();
	TargetingComp->ClearSoftTarget();

	bHitStopEnabled = false;
}

void ADMC_PlayerCharacter::HitStop()
{
	if (!bHitStopEnabled) return;

	CustomTimeDilation = HitStopTimeDilation;

	GetWorldTimerManager().SetTimer(HitStopTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		CustomTimeDilation = bRageActive && ComboData ? ComboData->RageTimeDilation : 1.0f;
	}), HitStopTime, false);
}

void ADMC_PlayerCharacter::EquipWeapon()
{
	if (!WeaponClass || EquippedWeapon) return;
	
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		
		EquippedWeapon = World->SpawnActor<ADMC_BaseWeapon>(WeaponClass, SpawnParams);
		
		if (EquippedWeapon)
		{
			EquippedWeapon->Equip(GetMesh(), WeaponSocketName, this, this);
		}
	}
}

void ADMC_PlayerCharacter::StartWeaponCollision(TSubclassOf<UDMC_DamageType> DamageType)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StartCollision(DamageType);
	}
}

void ADMC_PlayerCharacter::EndWeaponCollision()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->EndCollision();
	}
}

void ADMC_PlayerCharacter::LockOn()
{
	TargetingComp->LockOn();
}

void ADMC_PlayerCharacter::StopLockOn()
{
	TargetingComp->StopLockOn();
}

void ADMC_PlayerCharacter::SoftLockOn()
{
	TargetingComp->SoftLockOn();
}

void ADMC_PlayerCharacter::RotateToTarget()
{
	TargetingComp->RotateToTarget();
}

void ADMC_PlayerCharacter::SetAllowPhysicsRotation(bool bAllow)
{
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = bAllow;
}

void ADMC_PlayerCharacter::StopRotation()
{
	TargetingComp->StopRotation();
}

void ADMC_PlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	if (Hit.GetActor())
	{
		UClass* HitActorClass = Hit.GetActor()->GetClass();
		for (TSubclassOf<AActor> AllowedClass : CanLandClasses)
		{
			if (HitActorClass->IsChildOf(AllowedClass))
			{
				ResetDoubleJump();
				break;
			}
		}
	}

	bHitStopEnabled = false;
}

void ADMC_PlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADMC_PlayerCharacter::Look(const FInputActionValue& Value)
{
	if (TargetingComp->GetTargetActor()) return;
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

bool ADMC_PlayerCharacter::ExecuteAttack(UAnimMontage* Montage, float BufferAmount)
{
	if (!Montage) return false;

	BufferComponent->StopBuffer();
	BufferComponent->StartBuffer(BufferAmount);

	SetState(EDMC_PlayerState::ECS_Attack);
	SoftLockOn();
	PlayAnimMontage(Montage);

	return true;
}

void ADMC_PlayerCharacter::TryConsumeBufferedInput()
{
	// 1. Dodge has high priority
	if (bSaveDodge)
	{
		bSaveDodge = false;
		SetState(EDMC_PlayerState::ECS_Nothing);
		PerformDodge();
		return;
	}
	
	// 2. Check for Heavy -> Light Combo (Combo Starter)
	if (bSaveLightAttack && HeavyAttackIndex > 0)
	{
		bSaveLightAttack = false;
		SetState(EDMC_PlayerState::ECS_Nothing);
		PerformComboStarter();
		return;
	}
	
	// 3. Check for Light -> Heavy Combo (Combo Extender)
	if (bSaveHeavyAttack && ComboExtenderIndex > 0)
	{
		bSaveHeavyAttack = false;
		SetState(EDMC_PlayerState::ECS_Nothing);
		PerformComboExtender();
		return;
	}
	
	// 4. Regular Attacks
	if (bSaveLightAttack)
	{
		bSaveLightAttack = false;
		SetState(EDMC_PlayerState::ECS_Nothing);
		LightAttack();
		return;
	}
	
	if (bSaveHeavyAttack)
	{
		bSaveHeavyAttack = false;
		SetState(EDMC_PlayerState::ECS_Nothing);
		HeavyAttack();
		return;
	}
}

bool ADMC_PlayerCharacter::PerformLightAttack(int32 InAttackIndex)
{
	if (!ComboData) return false;

	const TArray<TObjectPtr<UAnimMontage>>& CurrentCombo = bRageActive ? ComboData->LightAttackRageCombo : ComboData->LightAttackCombo;

	if (!CurrentCombo.IsValidIndex(InAttackIndex)) return false;

	if (ExecuteAttack(CurrentCombo[InAttackIndex], ComboData->LightAttackBuffer))
	{
		LightAttackIndex++;
		if (LightAttackIndex >= CurrentCombo.Num())
		{
			LightAttackIndex = 0;
		}
		return true;
	}
	
	return false;
}

bool ADMC_PlayerCharacter::PerformHeavyAttack(int32 InAttackIndex)
{
	if (!ComboData || !ComboData->HeavyAttackCombo.IsValidIndex(InAttackIndex)) return false;

	if (ExecuteAttack(ComboData->HeavyAttackCombo[InAttackIndex], ComboData->HeavyAttackBuffer))
	{
		HeavyAttackIndex++;
		if (HeavyAttackIndex >= ComboData->HeavyAttackCombo.Num())
		{
			HeavyAttackIndex = 0;
		}
		return true;
	}
	
	return false;
}

bool ADMC_PlayerCharacter::PerformComboStarter()
{
	if (!ComboData || IsBusy() || GetCharacterMovement()->IsFalling()) return false;
	
	int32 HL_ComboStarterIndex = HeavyAttackIndex - 1;
	
	if (ComboData->ComboStarterMontages.IsValidIndex(HL_ComboStarterIndex))
	{
		if (ExecuteAttack(ComboData->ComboStarterMontages[HL_ComboStarterIndex], ComboData->StarterAttackBuffer))
		{
			ComboExtenderIndex = HeavyAttackIndex;
			ResetHeavyAttackVariables();
			bSaveLightAttack = false;
			return true;
		}
	}
	
	return false;
}

bool ADMC_PlayerCharacter::PerformComboExtender()
{
	if (!ComboData || IsBusy() || GetCharacterMovement()->IsFalling()) return false;

	int32 LH_FinisherIndex = ComboExtenderIndex - 1;
	if (ComboData->ComboExtenderMontages.IsValidIndex(LH_FinisherIndex))
	{
		if (ExecuteAttack(ComboData->ComboExtenderMontages[LH_FinisherIndex], ComboData->ExtenderAttackBuffer))
		{
			ResetLightAttackVariables();
			ResetHeavyAttackVariables();
			ComboExtenderIndex = 0;
			return true;
		}
	}

	return false;
}

bool ADMC_PlayerCharacter::SpecialAttack()
{
	if (bDodgeAttackEnabled && ComboData && ComboData->DodgeAttackMontage)
	{
		if (ExecuteAttack(ComboData->DodgeAttackMontage, ComboData->DodgeBufferAmount))
		{
			ResetLightAttackVariables();
			return true;
		}
	}

	if (bPerformChargeAttack)
	{
		SetState(EDMC_PlayerState::ECS_Attack);
		ResetLightAttackVariables();
		ResetHeavyAttackVariables();
		RotateToTarget();
		
		if (BufferComponent)
		{
			BufferComponent->StopBuffer();
			BufferComponent->StartBuffer(ComboData ? ComboData->ChargedBufferAmount : 5.0f);
		}
		
		if (ComboData && ComboData->ChargeAttackMontage)
		{
			PlayAnimMontage(ComboData->ChargeAttackMontage);
			return true;
		}
	}

	if (GetIsTargeting() && GetTargetActor())
	{
		if (IsBusy() || GetCharacterMovement()->IsFalling()) return false;

		FVector LastInput = GetCharacterMovement()->GetLastInputVector();
		float ForwardValue = FVector::DotProduct(GetActorForwardVector(), LastInput.GetSafeNormal());

		if (ForwardValue > 0.1f)
		{
			SetState(EDMC_PlayerState::ECS_Attack);
			ResetLightAttackVariables();
			ResetHeavyAttackVariables();
			RotateToTarget();
			
			if (BufferComponent)
			{
				BufferComponent->StopBuffer();
				BufferComponent->StartBuffer(ComboData ? ComboData->StingerAttackBuffer : 25.0f);
			}
			
			if (ComboData && ComboData->StingerAttackMontage)
			{
				PlayAnimMontage(ComboData->StingerAttackMontage);
				return true;
			}
		}
		else if (ForwardValue < -0.1f)
		{
			RotateToTarget();
			SetState(EDMC_PlayerState::ECS_Attack);
			
			return true;
		}
	}
	
	return false;
}

void ADMC_PlayerCharacter::PerformDodge()
{
	StopRotation();
	bPerformChargeAttack = false;
	TargetingComp->ClearSoftTarget();
	
	FVector LastInput = GetCharacterMovement()->GetLastInputVector();
	if (!LastInput.IsNearlyZero())
	{
		SetActorRotation(LastInput.Rotation());
	}
	
	BufferComponent->StopBuffer();
	
	if (ComboData)
	{
		BufferComponent->StartBuffer(ComboData->DodgeBufferAmount);
		
		SetState(EDMC_PlayerState::ECS_Dodge);
		
		if (ComboData->DodgeMontage)
		{
			PlayAnimMontage(ComboData->DodgeMontage);
		}
	}
}

void ADMC_PlayerCharacter::FinisherAttack()
{
	TArray<EDMC_PlayerState> StatesToIgnore;
	StatesToIgnore.Add(EDMC_PlayerState::ECS_Dodge);
	StatesToIgnore.Add(EDMC_PlayerState::ECS_Finisher);
	
	if (IsStateEqualToAny(StatesToIgnore)) return;

	if (GetIsTargeting() && GetTargetActor())
	{
		AActor* Target = GetTargetActor();
		
		float Distance = GetDistanceTo(Target);
		if (ComboData && Distance <= ComboData->FinisherAttackDistance)
		{
			if (ADMC_EnemyCharacterBase* Enemy = Cast<ADMC_EnemyCharacterBase>(Target))
			{
				if (Enemy->GetHealth() / Enemy->GetMaxHealth() > 0.1f) return;

				if (TargetingComp)
				{
					TargetingComp->StopRotation();
				}
				if (BufferComponent)
				{
					BufferComponent->StopBuffer();
				}
				Enemy->Finished(this);

				FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
				SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f));
				SetState(EDMC_PlayerState::ECS_Finisher);
				
				if (ComboData->FinisherAttackMontage)
				{
					PlayAnimMontage(ComboData->FinisherAttackMontage);
				}

				if (TargetingComp)
				{
					TargetingComp->StopLockOn();
				}
			}
		}
	}
}

void ADMC_PlayerCharacter::Rage()
{
	if (bRageActive || !ComboData || !ComboData->RageMontage) return;

	if (!GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
	{
		SetState(EDMC_PlayerState::ECS_GeneralActions);
		PlayAnimMontage(ComboData->RageMontage);

		if (ComboData->RageParticles_1 && Scene)
		{
			ActiveRageEmitter = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ComboData->RageParticles_1,
				Scene->GetComponentLocation(),
				GetActorRotation(),
				FVector(1.6f)
			);
		}

		GetWorldTimerManager().SetTimer(RageTimerHandle, this, &ADMC_PlayerCharacter::RageStage2, 1.0f, false);
	}
}

void ADMC_PlayerCharacter::RageStage2()
{
	if (ComboData && ComboData->RageParticles_2 && Scene)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ComboData->RageParticles_2,
			Scene->GetComponentLocation(),
			GetActorRotation(),
			FVector(1.0f)
		);
	}

	GetWorldTimerManager().SetTimer(RageTimerHandle, this, &ADMC_PlayerCharacter::RageStage3, 0.4f, false);
}

void ADMC_PlayerCharacter::RageStage3()
{
	if (ComboData && ComboData->RageOverlayMaterial)
	{
		GetMesh()->SetOverlayMaterial(ComboData->RageOverlayMaterial);
	}

	GetWorldTimerManager().SetTimer(RageTimerHandle, this, &ADMC_PlayerCharacter::RageStage4, 0.1f, false);
}

void ADMC_PlayerCharacter::RageMode()
{
	if (!ComboData) return;

	CustomTimeDilation = ComboData->RageTimeDilation;
	KatanaDamage = ComboData->RageDamageMultiplier;
	bRageActive = true;

	ResetLightAttackVariables();
	ResetHeavyAttackVariables();
	ComboExtenderIndex = 0;

	GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &ADMC_PlayerCharacter::StopRage, ComboData->RageDuration, false);
}

void ADMC_PlayerCharacter::StopRage()
{
	if (!bRageActive) return;
	
	GetWorldTimerManager().ClearTimer(DurationTimerHandle);

	CustomTimeDilation = 1.0f;
	KatanaDamage = 1.0f;
	bRageActive = false;

	ResetLightAttackVariables();
	ResetHeavyAttackVariables();
	ComboExtenderIndex = 0;

	if (GetMesh())
	{
		GetMesh()->SetOverlayMaterial(nullptr);
	}
}

void ADMC_PlayerCharacter::RageStage4()
{
	if (ActiveRageEmitter)
	{
		ActiveRageEmitter->DestroyComponent();
		ActiveRageEmitter = nullptr;
	}

	RageMode();
}

void ADMC_PlayerCharacter::LightAttackReleased()
{
	GetWorldTimerManager().ClearTimer(ChargeTimerHandle);
	if (bPerformChargeAttack)
	{
		SpecialAttack();
		bPerformChargeAttack = false;
	}
}

void ADMC_PlayerCharacter::OnChargeTimerFinished()
{
	TArray<EDMC_PlayerState> IgnoreStates;
	IgnoreStates.Add(EDMC_PlayerState::ECS_Dodge);
	IgnoreStates.Add(EDMC_PlayerState::ECS_Finisher);
    
	bool bValidState = !IsStateEqualToAny(IgnoreStates) && 
					  !GetCharacterMovement()->IsFalling() && 
					  !GetCharacterMovement()->IsFlying();
	if (bValidState)
	{
		bPerformChargeAttack = true;
		bSaveLightAttack = false;
	}
}

void ADMC_PlayerCharacter::ResetLightAttackVariables()
{
	LightAttackIndex = 0;
	bSaveLightAttack = false;
	bDodgeAttackEnabled = false;
	bPerformChargeAttack = false;
}

void ADMC_PlayerCharacter::ResetHeavyAttackVariables()
{
	HeavyAttackIndex = 0;
	bSaveHeavyAttack = false;
}

bool ADMC_PlayerCharacter::GetIsTargeting() const
{
	return TargetingComp ? TargetingComp->IsTargeting() : false;
}

AActor* ADMC_PlayerCharacter::GetSoftTarget() const
{
	return TargetingComp ? TargetingComp->GetSoftTarget() : nullptr;
}

AActor* ADMC_PlayerCharacter::GetCombatTarget() const
{
	return TargetingComp ? TargetingComp->GetTargetActor() : nullptr;
}