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
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DamageTypes/DMC_DamageType.h"
#include "Kismet/KismetMathLibrary.h"

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
	
	if (bActiveCollision && EquippedWeapon && EquippedWeapon->GetWeaponMesh())
	{
		const FVector TraceStart = EquippedWeapon->GetWeaponMesh()->GetSocketLocation(FName("Start"));
		const FVector TraceEnd = EquippedWeapon->GetWeaponMesh()->GetSocketLocation(FName("End"));
		
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);
		
		TArray<FHitResult> OutHits;
		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
			GetWorld(),
			TraceStart,
			TraceEnd,
			20.f,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			OutHits,
			true
		);
		
		if (bHit)
		{
			for (const FHitResult& Hit : OutHits)
			{
				AActor* HitActor = Hit.GetActor();
				if (IsValid(HitActor) && !AlreadyHitActors.Contains(HitActor))
				{
					AlreadyHitActors.AddUnique(HitActor);
					UGameplayStatics::ApplyDamage(
						HitActor,
						1.f,
						GetController(),
						this,
						DamageTypeClass
					);
				}
			}
		}
	}
	
	if (bIsTargeting && IsValid(TargetActor))
	{
		TArray<EDMC_PlayerState> DodgeState;
		DodgeState.Add(EDMC_PlayerState::ECS_Dodge);
		if (!IsStateEqualToAny(DodgeState))
		{
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;

			if (AController* PC = GetController())
			{
				FRotator CurrentRotation = PC->GetControlRotation(); 
                
				FVector TargetLoc = TargetActor->GetActorLocation();
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLoc);
				FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 5.0f);
                
				PC->SetControlRotation(NewRotation);
			}
		}
		else
		{
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}
	
	if (RotationTimeline.IsPlaying())
	{
		RotationTimeline.TickTimeline(DeltaTime);
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
	
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::LockOn);
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::StopLockOn);
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
	if (bIsTargeting) return;
	
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
		if (!GetCharacterMovement()->IsFalling())
		{
			ResetHeavyAttackVariables();
			PerformLightAttack(LightAttackIndex);
		}
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
			SoftLockOn();
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
		if (!GetCharacterMovement()->IsFalling())
		{
			ResetLightAttackVariables();
			PerformHeavyAttack(HeavyAttackIndex);
		}
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
			SoftLockOn();
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
	
	if (IsStateEqualToAny(StatesToCheck) || GetCharacterMovement()->IsFalling()) return false;
	
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
			SoftLockOn();
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
	
	if (IsStateEqualToAny(StatesToCheck) || GetCharacterMovement()->IsFalling()) return false;

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
			SoftLockOn();
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
		if (!GetCharacterMovement()->IsFalling())
		{
			PerformDodge();
		}
	}
}

void ADMC_PlayerCharacter::PerformDodge()
{
	StopBuffer();
	StartBuffer(DodgeBufferAmount);
	
	SetState(EDMC_PlayerState::ECS_Dodge);
	PlayAnimMontage(DodgeMontage);
}

void ADMC_PlayerCharacter::LockOn()
{
	bInputHold = true;
	
	FVector Start = GetActorLocation();
	FVector End = Start + (GetFollowCamera()->GetForwardVector() * 1000.f);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
    
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	
	FHitResult OutHit;
	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		150.f,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true
	);
	
	if (bHit && IsValid(OutHit.GetActor()))
	{
		bIsTargeting = true;
		TargetActor = OutHit.GetActor();
		
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->MaxWalkSpeed = 250.f;
	}
	else
	{
		bIsTargeting = false;
		TargetActor = nullptr;
		
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void ADMC_PlayerCharacter::StopLockOn()
{
	bInputHold = false;
	bIsTargeting = false;
	TargetActor = nullptr;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ADMC_PlayerCharacter::SoftLockOn()
{
	if (bIsTargeting && IsValid(SoftTarget))
	{
		SoftTarget = nullptr;
		return;
	}
	
	FVector LastInput = GetCharacterMovement()->GetLastInputVector();
	FVector Start = GetActorLocation();
	FVector End = Start + (LastInput * 1000.f);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	
	FHitResult OutHit;
	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		100.f,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true
	);
	
	if (bHit && IsValid(OutHit.GetActor()))
	{
		SoftTarget = OutHit.GetActor();
	}
	else
	{
		SoftTarget = nullptr;
	}
}

void ADMC_PlayerCharacter::HandleRotationTimelineProgress(float Value)
{
	AActor* ActualTarget = IsValid(TargetActor) ? TargetActor.Get() : SoftTarget.Get();
	
	if (!ActualTarget) return;
	
	FVector TargetLocation = ActualTarget->GetActorLocation();
	FVector MyLocation = GetActorLocation();
	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);
	FRotator CurrentRot = GetActorRotation();
	
	FRotator TargetRot = FRotator(CurrentRot.Pitch, LookAtRot.Yaw, CurrentRot.Roll);
	FRotator NewRot = FMath::Lerp(CurrentRot, TargetRot, Value);
    
	SetActorRotation(NewRot);
}

void ADMC_PlayerCharacter::RotateToTarget()
{
	if (IsValid(TargetActor) || IsValid(SoftTarget))
	{
		if (RotationCurve)
		{
			FOnTimelineFloat ProgressFunction;
			ProgressFunction.BindUFunction(this, FName("HandleRotationTimelineProgress"));
			RotationTimeline.AddInterpFloat(RotationCurve, ProgressFunction);
			RotationTimeline.SetLooping(false);
			RotationTimeline.PlayFromStart();
		}
	}
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

void ADMC_PlayerCharacter::StartWeaponCollision()
{
	AlreadyHitActors.Empty();
	bActiveCollision = true;
}

void ADMC_PlayerCharacter::EndWeaponCollision()
{
	bActiveCollision = false;
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