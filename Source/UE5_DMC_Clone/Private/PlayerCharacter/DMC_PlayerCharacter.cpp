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
	
	CurrentState = EDMC_PlayerState::ECS_Nothing;
}

void ADMC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Automatically Equip Weapon at start
	EquipWeapon();
}

void ADMC_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsBuffering && BufferCurve)
	{
		BufferTimeElapsed += DeltaTime;
		
		if (BufferTimeElapsed <= BufferDuration)
		{
			float Alpha = BufferCurve->GetFloatValue(BufferTimeElapsed);
			
			FVector CurrentLocation = GetActorLocation();
			FVector Forward = GetActorForwardVector();
			FVector Offset = Forward * CurrentBufferAmount * Alpha * DeltaTime * 60.f;
			
			FHitResult Hit;
			SetActorLocation(CurrentLocation + Offset, true, &Hit);
		}
		else
		{
			StopBuffer();
		}
	}
}

void ADMC_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADMC_PlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADMC_PlayerCharacter::Look);
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::LightAttack);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::HeavyAttack);
		
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::Dodge);
	}
}

void ADMC_PlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Fet forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// Get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADMC_PlayerCharacter::Look(const FInputActionValue& Value)
{
	// Input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADMC_PlayerCharacter::Jump()
{
	TArray<EDMC_PlayerState> StatesToCheck;
	StatesToCheck.Add(EDMC_PlayerState::ECS_Attack);
	StatesToCheck.Add(EDMC_PlayerState::ECS_Dodge);
	if (IsStateEqualToAny(StatesToCheck)) return;
	
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

void ADMC_PlayerCharacter::EquipWeapon()
{
	if (!WeaponClass || EquippedWeapon) return;
	
	if (WeaponClass)
	{
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
}

void ADMC_PlayerCharacter::LightAttack()
{
	bSaveHeavyAttack = false;
	bSaveDodge = false;
	
	TArray<EDMC_PlayerState> StatesToCheck;
	StatesToCheck.Add(EDMC_PlayerState::ECS_Attack);
	StatesToCheck.Add(EDMC_PlayerState::ECS_Dodge);
	
	if (IsStateEqualToAny(StatesToCheck))
	{
		bSaveLightAttack = true;
	}
	else
	{
		ResetHeavyAttackVariables();
		PerformLightAttack(LightAttackIndex);
	}
}

bool ADMC_PlayerCharacter::PerformLightAttack(int32 InAttackIndex)
{
	if (LightAttackCombo.IsValidIndex(InAttackIndex))
	{
		UAnimMontage* L_AttackMontage = LightAttackCombo[InAttackIndex];
		
		if (L_AttackMontage)
		{
			StopBuffer();
			StartBuffer(LightAttackBufferAmount);
			
			SetState(EDMC_PlayerState::ECS_Attack);
			PlayAnimMontage(L_AttackMontage);
			
			LightAttackIndex++;
			
			if (LightAttackIndex >= LightAttackCombo.Num())
			{
				LightAttackIndex = 0;
			}
			
			return true;
		}
	}
	
	return false;
}

void ADMC_PlayerCharacter::HeavyAttack()
{
	bSaveLightAttack = false;
	bSaveDodge = false;
	
	TArray<EDMC_PlayerState> StatesToCheck;
	StatesToCheck.Add(EDMC_PlayerState::ECS_Attack);
	StatesToCheck.Add(EDMC_PlayerState::ECS_Dodge);
	
	if (IsStateEqualToAny(StatesToCheck))
	{
		bSaveHeavyAttack = true;
	}
	else
	{
		ResetLightAttackVariables();
		PerformHeavyAttack(HeavyAttackIndex);
	}
}

bool ADMC_PlayerCharacter::PerformHeavyAttack(int32 InAttackIndex)
{
	if (HeavyAttackCombo.IsValidIndex(InAttackIndex))
	{
		UAnimMontage* H_AttackMontage = HeavyAttackCombo[InAttackIndex];
		
		if (H_AttackMontage)
		{
			StopBuffer();
			StartBuffer(HeavyAttackBufferAmount);
			
			SetState(EDMC_PlayerState::ECS_Attack);
			PlayAnimMontage(H_AttackMontage);
			
			HeavyAttackIndex++;
			
			if (HeavyAttackIndex >= HeavyAttackCombo.Num())
			{
				HeavyAttackIndex = 0;
			}
			
			return true;
		}
	}
	
	return false;
}

bool ADMC_PlayerCharacter::PerformComboStarter()
{
	TArray<EDMC_PlayerState> StatesToCheck;
	StatesToCheck.Add(EDMC_PlayerState::ECS_Attack);
	StatesToCheck.Add(EDMC_PlayerState::ECS_Dodge);
	if (IsStateEqualToAny(StatesToCheck)) return false;
	
	int32 HL_ComboStarterIndex = HeavyAttackIndex - 1;
	
	if (ComboStarterMontages.IsValidIndex(HL_ComboStarterIndex))
	{
		UAnimMontage* HL_AttackMontage = ComboStarterMontages[HL_ComboStarterIndex];
		
		if (HL_AttackMontage)
		{
			ComboExtenderIndex = HeavyAttackIndex;
			
			StopBuffer();
			StartBuffer(StarterAttackBufferAmount);
			
			bSaveHeavyAttack = false;
			bSaveLightAttack = false;
			
			SetState(EDMC_PlayerState::ECS_Attack);
			PlayAnimMontage(HL_AttackMontage);
			
			return true;
		}
	}
	
	return false;
}

bool ADMC_PlayerCharacter::PerformComboExtender()
{
	TArray<EDMC_PlayerState> StatesToCheck;
	StatesToCheck.Add(EDMC_PlayerState::ECS_Attack);
	StatesToCheck.Add(EDMC_PlayerState::ECS_Dodge);
	if (IsStateEqualToAny(StatesToCheck)) return false;

	int32 LH_FinisherIndex = ComboExtenderIndex - 1;
	if (ComboExtenderMontages.IsValidIndex(LH_FinisherIndex))
	{
		UAnimMontage* LH_AttackMontage = ComboExtenderMontages[LH_FinisherIndex];
		if (LH_AttackMontage)
		{
			ResetLightAttackVariables();
			ResetHeavyAttackVariables();

			StopBuffer();
			StartBuffer(ExtenderAttackBufferAmount);

			SetState(EDMC_PlayerState::ECS_Attack);
			PlayAnimMontage(LH_AttackMontage);
			return true;
		}
	}

	return false;
}

void ADMC_PlayerCharacter::Dodge()
{
	TArray<EDMC_PlayerState> StatesToCheck;
	StatesToCheck.Add(EDMC_PlayerState::ECS_Dodge);
	
	if (IsStateEqualToAny(StatesToCheck))
	{
		bSaveDodge = true;
	}
	else
	{
		PerformDodge();
	}
}

void ADMC_PlayerCharacter::PerformDodge()
{
	StopBuffer();
	StartBuffer(DodgeBufferAmount);
	
	SetState(EDMC_PlayerState::ECS_Dodge);
	PlayAnimMontage(DodgeMontage);
}

void ADMC_PlayerCharacter::StartBuffer(float Amount)
{
	CurrentBufferAmount = Amount;
	BufferTimeElapsed = 0.f;
	bIsBuffering = true;
}

void ADMC_PlayerCharacter::StopBuffer()
{
	bIsBuffering = false;
}

void ADMC_PlayerCharacter::SaveLightAttack()
{
	if (bSaveLightAttack)
	{
		bSaveLightAttack = false;

		TArray<EDMC_PlayerState> AttackStates;
		AttackStates.Add(EDMC_PlayerState::ECS_Attack);
		
		if (IsStateEqualToAny(AttackStates))
		{
			SetState(EDMC_PlayerState::ECS_Nothing);
		}
		
		LightAttack();
	}
	else if (bSaveHeavyAttack && ComboExtenderIndex > 0)
	{
		TArray<EDMC_PlayerState> AttackStates;
		AttackStates.Add(EDMC_PlayerState::ECS_Attack);
			
		if (IsStateEqualToAny(AttackStates))
		{
			SetState(EDMC_PlayerState::ECS_Nothing);
		}
			
		PerformComboExtender();
	}
}

void ADMC_PlayerCharacter::SaveHeavyAttack()
{
	if (bSaveHeavyAttack)
	{
		bSaveHeavyAttack = false;
		
		TArray<EDMC_PlayerState> AttackStates;
		AttackStates.Add(EDMC_PlayerState::ECS_Attack);
		
		if (IsStateEqualToAny(AttackStates))
		{
			SetState(EDMC_PlayerState::ECS_Nothing);
		}
		
		HeavyAttack();
	}
	else if (bSaveLightAttack && HeavyAttackIndex > 0)
	{
		TArray<EDMC_PlayerState> AttackStates;
		AttackStates.Add(EDMC_PlayerState::ECS_Attack);
		
		if (IsStateEqualToAny(AttackStates))
		{
			SetState(EDMC_PlayerState::ECS_Nothing);
		}
		
		PerformComboStarter();
	}
}

void ADMC_PlayerCharacter::SaveDodge()
{
	if (bSaveDodge)
	{
		bSaveDodge = false;
		
		TArray<EDMC_PlayerState> StatesToCheck;
		StatesToCheck.Add(EDMC_PlayerState::ECS_Dodge);
		
		if (IsStateEqualToAny(StatesToCheck))
		{
			SetState(EDMC_PlayerState::ECS_Dodge);
		}
		
		PerformDodge();
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

void ADMC_PlayerCharacter::ResetState()
{
	SetState(EDMC_PlayerState::ECS_Nothing);
	ResetLightAttackVariables();
	ResetHeavyAttackVariables();
	bSaveDodge = false;
	StopBuffer();
	ComboExtenderIndex = 0;
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