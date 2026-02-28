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
#include "Components/DMC_FinisherComponent.h"
#include "Components/DMC_CombatComponent.h"
#include "Components/DMC_TargetingComponent.h"
#include "Components/DMC_RageComponent.h"
#include "Enemies/DMC_EnemyCharacterBase.h"
#include "Data/DMC_ComboDataAsset.h"
#include "Items/DMC_BaseWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DamageTypes/DMC_DamageType.h"
#include "Engine/DamageEvents.h"
#include "UI/DMC_PlayerHUD.h"
#include "Blueprint/UserWidget.h"

ADMC_PlayerCharacter::ADMC_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
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
	
	BufferComponent = CreateDefaultSubobject<UDMC_CombatBufferComponent>(TEXT("BufferComponent"));
	RageComp = CreateDefaultSubobject<UDMC_RageComponent>(TEXT("RageComp"));
	TargetingComp = CreateDefaultSubobject<UDMC_TargetingComponent>(TEXT("TargetingComp"));
	FinisherComp = CreateDefaultSubobject<UDMC_FinisherComponent>(TEXT("FinisherComp"));
	CombatComp = CreateDefaultSubobject<UDMC_CombatComponent>(TEXT("CombatComp"));
	
	CurrentState = EDMC_PlayerState::ECS_Nothing;
}

void ADMC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (ComboData)
	{
		Health = ComboData->MaxHealth;
	}
	
	
	if (PlayerHUDClass)
	{
		PlayerHUD = CreateWidget<UDMC_PlayerHUD>(GetWorld(), PlayerHUDClass);
		if (PlayerHUD)
		{
			PlayerHUD->AddToViewport();
			UpdateHUD();
		}
	}

	EquipWeapon();
}

void ADMC_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADMC_PlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (bDead || IsDodging()) return 0.f;

	// Parry Detection
	if (IsParrying())
	{
		HandleSuccessfulParry(DamageCauser);
		return 0.f;
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		SpawnHitFX(DamageCauser, PointDamageEvent->HitInfo);
	}

	Health = FMath::Clamp(Health - DamageAmount, 0.f, ComboData ? ComboData->MaxHealth : 100.f);

	if (Health <= 0.f)
	{
		Death();
	}
	
	UpdateHUD();

	if (DamageEvent.DamageTypeClass)
	{
		UDMC_DamageType* DamageTypeObject = Cast<UDMC_DamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
		if (DamageTypeObject)
		{
			PlayHitReaction(DamageTypeObject->DamageType);
		}
	}

	return DamageAmount;
}

void ADMC_PlayerCharacter::SpawnHitFX(AActor* DamageCauser, const FHitResult& HitResult)
{
	if (!ComboData || !ComboData->HitVFX || !DamageCauser) return;

	FRotator YawRot = UKismetMathLibrary::FindLookAtRotation(DamageCauser->GetActorLocation(), GetActorLocation());
	FRotator TiltRot = UKismetMathLibrary::FindLookAtRotation(HitResult.ImpactPoint, HitResult.TraceEnd);
	FRotator FinalRot = FRotator(TiltRot.Pitch, YawRot.Yaw + 90.f, TiltRot.Roll);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ComboData->HitVFX, HitResult.ImpactPoint, FinalRot);
}

void ADMC_PlayerCharacter::PlayHitReaction(EDMC_DamageType DamageDirection)
{
	if (IsDodging() || IsBusy() || !ComboData) return;

	if (ComboData->HitReactionMap.Contains(DamageDirection))
	{
		const FDMC_HitReactionData& Data = ComboData->HitReactionMap[DamageDirection];

		if (BufferComponent)
		{
			BufferComponent->StopBuffer();
			BufferComponent->StartBuffer(Data.PushbackAmount);
		}

		if (Data.HitReactMontage)
		{
			PlayAnimMontage(Data.HitReactMontage);
		}
	}
}

void ADMC_PlayerCharacter::Death()
{
	if (bDead) return;
	bDead = true;

	if (ComboData && ComboData->DeathMontage)
	{
		PlayAnimMontage(ComboData->DeathMontage);
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	}

	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Keep visible but ignore combat
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}

	// Disable input
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &ADMC_PlayerCharacter::ShowGameOver, 3.0f, false);
}

void ADMC_PlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (Hit.GetActor())
	{
		for (TSubclassOf<AActor> AllowedClass : CanLandClasses)
		{
			if (Hit.GetActor()->IsA(AllowedClass)) { ResetDoubleJump(); break; }
		}
	}
	bHitStopEnabled = false;
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
		
		EnhancedInputComponent->BindAction(RageAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::Rage);
		EnhancedInputComponent->BindAction(StopRageAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::StopRage);
		
		EnhancedInputComponent->BindAction(ModifierAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::ModifierPressed);
		EnhancedInputComponent->BindAction(ModifierAction, ETriggerEvent::Completed, this, &ADMC_PlayerCharacter::ModifierReleased);

		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &ADMC_PlayerCharacter::ParryPressed);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Completed, this, &ADMC_PlayerCharacter::ParryReleased);
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

void ADMC_PlayerCharacter::ResetLightAttackVariables()
{
	if (CombatComp)
	{
		CombatComp->ResetLightCombo();
		CombatComp->SetDodgeAttackEnabled(false);
		CombatComp->SetPerformChargeAttack(false);
	}
}

void ADMC_PlayerCharacter::ResetHeavyAttackVariables()
{
	if (CombatComp) CombatComp->ResetHeavyCombo();
}

void ADMC_PlayerCharacter::ResetState()
{
	SetState(EDMC_PlayerState::ECS_Nothing);
	if (BufferComponent)
	{
		BufferComponent->ClearInputBuffer();
		BufferComponent->StopBuffer();
	}
	ResetLightAttackVariables();
	ResetHeavyAttackVariables();
	
	if (TargetingComp)
	{
		TargetingComp->StopRotation();
		TargetingComp->ClearSoftTarget();
	}
	ResetParryState();
	bHitStopEnabled = false;
}

void ADMC_PlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D Moved = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator YawRot(0, Controller->GetControlRotation().Yaw, 0);
		AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::X), Moved.Y);
		AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y), Moved.X);
	}
}

void ADMC_PlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D Looked = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(Looked.X);
		AddControllerPitchInput(Looked.Y);
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
			if (DoubleJumpMontage) PlayAnimMontage(DoubleJumpMontage);
			LaunchCharacter(FVector(0.f, 0.f, DoubleJumpLaunchVelocity), false, true);
		}
	}
	else Super::Jump();
}

void ADMC_PlayerCharacter::LightAttack()
{
	if (CombatComp)
	{
		if (CurrentState == EDMC_PlayerState::ECS_Dodge)
		{
			CombatComp->SetDodgeAttackEnabled(true);
		}
		
		bHitStopEnabled = false;
		if (!CombatComp->SpecialAttack()) // Only do normal attack if no special attack was triggered
		{
			CombatComp->PerformLightAttack();
		}
	}
	
	GetWorldTimerManager().SetTimer(ChargeTimerHandle, this, &ADMC_PlayerCharacter::OnChargeTimerFinished, 0.5f, false);
}

void ADMC_PlayerCharacter::LightAttackReleased()
{
	GetWorldTimerManager().ClearTimer(ChargeTimerHandle);
	if (CombatComp && CombatComp->GetPerformChargeAttack())
	{
		CombatComp->SpecialAttack();
		CombatComp->SetPerformChargeAttack(false);
	}
}

void ADMC_PlayerCharacter::OnChargeTimerFinished()
{
	TArray<EDMC_PlayerState> IgnoreList = { EDMC_PlayerState::ECS_Dodge, EDMC_PlayerState::ECS_Finisher };
	if (!IsStateEqualToAny(IgnoreList) && !GetCharacterMovement()->IsFalling())
	{
		if (CombatComp) CombatComp->SetPerformChargeAttack(true);
	}
}

void ADMC_PlayerCharacter::HeavyAttack()
{
	if (CombatComp)
	{
		CombatComp->PerformHeavyAttack();
	}
}

void ADMC_PlayerCharacter::Dodge()
{
	if (IsDodging() || CurrentState == EDMC_PlayerState::ECS_Finisher)
	{
		if (BufferComponent) BufferComponent->BufferInput(EDMC_BufferedInput::EBI_Dodge);
	}
	else
	{
		if (CombatComp && !GetCharacterMovement()->IsFalling())
		{
			CombatComp->PerformDodge();
		}
	}
}

void ADMC_PlayerCharacter::FinisherAttack()
{
	if (FinisherComp) FinisherComp->TryExecuteFinisher();
}

void ADMC_PlayerCharacter::ParryPressed()
{
	if (IsBusy() || CurrentState == EDMC_PlayerState::ECS_Finisher || GetCharacterMovement()->IsFalling()) return;

	if (ComboData)
	{
		const FDMC_ParryData& Data = ComboData->ParryData;
		if (Data.ParryStartMontage)
		{
			PlayAnimMontage(Data.ParryStartMontage);
			SetState(EDMC_PlayerState::ECS_Parry);
		}
	}
}

void ADMC_PlayerCharacter::ParryReleased()
{
	if (CurrentState != EDMC_PlayerState::ECS_Parry) return;

	if (ComboData)
	{
		const FDMC_ParryData& Data = ComboData->ParryData;
		if (Data.ParryEndMontage)
		{
			PlayAnimMontage(Data.ParryEndMontage);
		}
	}
	
	ResetParryState();
}

void ADMC_PlayerCharacter::ReturnToParryPose()
{
	if (CurrentState != EDMC_PlayerState::ECS_Parry || !ComboData) return;

	const FDMC_ParryData& Data = ComboData->ParryData;
	if (Data.ParryStartMontage)
	{
		PlayAnimMontage(Data.ParryStartMontage);
	}
}

void ADMC_PlayerCharacter::HandleSuccessfulParry(AActor* DamageCauser)
{
	if (!ComboData) return;

	const FDMC_ParryData& Data = ComboData->ParryData;

	// Visual Feedback
	if (DamageCauser && Data.ParryFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Data.ParryFX, GetActorLocation() + GetActorForwardVector() * 50.f);
	}

	// Rotate towards attacker
	if (TargetingComp && DamageCauser)
	{
		TargetingComp->RotateTowards(DamageCauser);
	}

	// Audio Feedback
	if (Data.ParrySound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Data.ParrySound, GetActorLocation());
	}

	// Success Animation (clash/recoil)
	if (Data.ParrySuccessMontage)
	{
		float Duration = PlayAnimMontage(Data.ParrySuccessMontage);
		
		// Re-trigger the parry pose after the clash finishes, if still holding the key
		GetWorldTimerManager().SetTimer(ParryTimerHandle, this, &ADMC_PlayerCharacter::ReturnToParryPose, Duration, false);
	}
	else
	{
		ReturnToParryPose();
	}

	// Stagger Enemy
	if (ADMC_EnemyCharacterBase* Enemy = Cast<ADMC_EnemyCharacterBase>(DamageCauser))
	{
		Enemy->HandleParried(this);
	}

	// Note: We don't call ResetParryState() here because the user is still holding the key.
	// The state will be reset when they release the key.
}

void ADMC_PlayerCharacter::ResetParryState()
{
	if (CurrentState == EDMC_PlayerState::ECS_Parry)
	{
		SetState(EDMC_PlayerState::ECS_Nothing);
	}
	GetWorldTimerManager().ClearTimer(ParryTimerHandle);
}

void ADMC_PlayerCharacter::Rage()
{
	if (RageComp) RageComp->StartRage();
}

void ADMC_PlayerCharacter::StopRage()
{
	if (RageComp) RageComp->StopRage();
}

void ADMC_PlayerCharacter::ModifierPressed()
{
	bModifierHeld = true;
}

void ADMC_PlayerCharacter::ModifierReleased()
{
	bModifierHeld = false;
}

void ADMC_PlayerCharacter::SaveLightAttack()
{
	if (CombatComp) CombatComp->TryConsumeBufferedInput();
}

void ADMC_PlayerCharacter::SaveHeavyAttack()
{
	if (CombatComp) CombatComp->TryConsumeBufferedInput();
}

void ADMC_PlayerCharacter::SaveDodge()
{
	if (CombatComp) CombatComp->TryConsumeBufferedInput();
}

void ADMC_PlayerCharacter::HitStop()
{
	if (!bHitStopEnabled) return;
	CustomTimeDilation = HitStopTimeDilation;
	GetWorldTimerManager().SetTimer(HitStopTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		CustomTimeDilation = IsRaging() && ComboData ? ComboData->RageTimeDilation : 1.0f;
	}), HitStopTime, false);
}

void ADMC_PlayerCharacter::StartWeaponCollision(TSubclassOf<class UDMC_DamageType> DamageType)
{
	if (EquippedWeapon) EquippedWeapon->StartCollision(DamageType);
}

void ADMC_PlayerCharacter::EndWeaponCollision()
{
	if (EquippedWeapon) EquippedWeapon->EndCollision();
}

void ADMC_PlayerCharacter::SetAllowPhysicsRotation(bool bAllow)
{
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = bAllow;
}

void ADMC_PlayerCharacter::RotateToTarget()
{
	if (TargetingComp) TargetingComp->RotateToTarget();
}

void ADMC_PlayerCharacter::StopRotation()
{
	if (TargetingComp) TargetingComp->StopRotation();
}

void ADMC_PlayerCharacter::SoftLockOn()
{
	if (TargetingComp) TargetingComp->SoftLockOn();
}

bool ADMC_PlayerCharacter::IsRaging() const
{
	return RageComp ? RageComp->IsRageActive() : false;
}

bool ADMC_PlayerCharacter::GetIsTargeting() const
{
	return TargetingComp ? TargetingComp->IsTargeting() : false;
}

TObjectPtr<AActor> ADMC_PlayerCharacter::GetSoftTarget() const
{
	return TargetingComp ? TargetingComp->GetSoftTarget() : nullptr;
}

TObjectPtr<AActor> ADMC_PlayerCharacter::GetCombatTarget() const
{
	return TargetingComp ? TargetingComp->GetTargetActor() : nullptr;
}

void ADMC_PlayerCharacter::EquipWeapon()
{
	if (!WeaponClass || EquippedWeapon) return;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	EquippedWeapon = GetWorld()->SpawnActor<ADMC_BaseWeapon>(WeaponClass, SpawnParams);
	if (EquippedWeapon) EquippedWeapon->Equip(GetMesh(), WeaponSocketName, this, this);
}

void ADMC_PlayerCharacter::UpdateHUD()
{
	if (PlayerHUD && ComboData)
	{
		PlayerHUD->SetHealthPercent(Health / ComboData->MaxHealth);
	}
}

void ADMC_PlayerCharacter::ShowGameOver()
{
	if (!GameOverWidgetClass) return;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();

			// Set Input Mode to UI Only and show cursor
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}