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
#include "Items/DMC_BaseWeapon.h"

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
	
	BufferComponent = CreateDefaultSubobject<UDMC_CombatBufferComponent>(TEXT("CombatBuffer"));
	TargetingComp = CreateDefaultSubobject<UDMC_TargetingComponent>(TEXT("TargetingComponent"));
	
	CurrentState = EDMC_PlayerState::ECS_Nothing;
}

void ADMC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	EquipWeapon();

	// Setup Upward Timeline
	if (ComboData && ComboData->LaunchUpCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleUpwardMovement"));
		UpwardTimeline.AddInterpFloat(ComboData->LaunchUpCurve, ProgressFunction);
		UpwardTimeline.SetLooping(false);
	}
}

void ADMC_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UpwardTimeline.IsPlaying())
	{
		UpwardTimeline.TickTimeline(DeltaTime);
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
		
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::LightAttackPressed);
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Completed, this, &ADMC_PlayerCharacter::LightAttackReleased);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::HeavyAttack);
		
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::Dodge);
	
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::LockOn);
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Completed, this, &ADMC_PlayerCharacter::StopLockOn);
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

void ADMC_PlayerCharacter::LightAttackPressed()
{
	bLightInputHeld = true;
	LightAttack();
}

void ADMC_PlayerCharacter::LightAttackReleased()
{
	bLightInputHeld = false;
}

void ADMC_PlayerCharacter::LightAttack()
{
	bSaveHeavyAttack = false;
	bSaveDodge = false;
	
	if (IsBusy())
	{
		bSaveLightAttack = true;
	}
	else
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			if (CanLaunch())
			{
				ExecuteAttack(ComboData->LaunchAttackMontage, ComboData->LaunchBuffer);
				return;
			}

			ResetHeavyAttackVariables();
			PerformLightAttack(LightAttackIndex);
		}
	}
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
	if (IsDodging())
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
	if (IsBusy()) return;
	
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
}

void ADMC_PlayerCharacter::LaunchCharacterUp()
{
	// Launch the target first (if any) - Instant lift
	if (AActor* Target = GetCombatTarget())
	{
		if (ACharacter* TargetCharacter = Cast<ACharacter>(Target))
		{
			TargetCharacter->LaunchCharacter(FVector(0.f, 0.f, 1200.f), false, true);
		}
	}

	// Launch player smoothly with Timeline if button is held
	if (bLightInputHeld && ComboData && ComboData->LaunchUpCurve)
	{
		LaunchStartLocation = GetActorLocation();
		LaunchTargetLocation = LaunchStartLocation + FVector(0.f, 0.f, ComboData->LaunchUpDistance);
		
		UpwardTimeline.PlayFromStart();
	}
}

void ADMC_PlayerCharacter::HandleUpwardMovement(float Value)
{
	FVector NewLocation = FMath::Lerp(LaunchStartLocation, LaunchTargetLocation, Value);
	SetActorLocation(NewLocation, true);
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
}

void ADMC_PlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	CurrentMovementInput = MovementVector;

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
	if (TargetingComp->IsTargeting()) return;
	
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
	if (!ComboData || !ComboData->LightAttackCombo.IsValidIndex(InAttackIndex)) return false;

	if (ExecuteAttack(ComboData->LightAttackCombo[InAttackIndex], ComboData->LightAttackBuffer))
	{
		LightAttackIndex++;
		if (LightAttackIndex >= ComboData->LightAttackCombo.Num())
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

void ADMC_PlayerCharacter::PerformDodge()
{
	StopRotation();
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

void ADMC_PlayerCharacter::ResetLightAttackVariables()
{
	LightAttackIndex = 0;
	bSaveLightAttack = false;
}

void ADMC_PlayerCharacter::ResetHeavyAttackVariables()
{
	HeavyAttackIndex = 0;
	bSaveHeavyAttack = false;
}

bool ADMC_PlayerCharacter::CanLaunch() const
{
	return GetIsTargeting() && CurrentMovementInput.Y <= -0.7f;
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