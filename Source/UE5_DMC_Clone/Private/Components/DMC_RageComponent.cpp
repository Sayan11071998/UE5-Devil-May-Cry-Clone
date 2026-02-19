#include "Components/DMC_RageComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/DMC_CombatInterface.h"
#include "Data/DMC_ComboDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h" // Needed for state check

UDMC_RageComponent::UDMC_RageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDMC_RageComponent::BeginPlay()
{
	Super::BeginPlay();
}

// ~ Begin Rage API
void UDMC_RageComponent::StartRage()
{
	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	if (!Player || bRageActive) return;

	UDMC_ComboDataAsset* ComboData = Player->GetComboData();
	if (!ComboData || !ComboData->RageMontage) return;

	if (!Player->GetCharacterMovement()->IsFalling() && !Player->GetCharacterMovement()->IsFlying())
	{
		Player->SetState(EDMC_PlayerState::ECS_GeneralActions);
		Player->PlayAnimMontage(ComboData->RageMontage);

		CurrentStageIndex = 0;
		ExecuteNextRageStage();
	}
}

void UDMC_RageComponent::StopRage()
{
	if (!bRageActive) return;

	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	if (Player)
	{
		Player->CustomTimeDilation = 1.0f;
		Player->SetKatanaDamage(1.0f);
		
		if (Player->GetMesh())
		{
			Player->GetMesh()->SetOverlayMaterial(nullptr);
		}
		
		Player->ResetLightAttackVariables();
		Player->ResetHeavyAttackVariables();
	}

	bRageActive = false;
	GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
}
// ~ End Rage API

// ~ Begin Internal Implementation
void UDMC_RageComponent::ExecuteNextRageStage()
{
	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	if (!Player) return;

	UDMC_ComboDataAsset* ComboData = Player->GetComboData();
	if (!ComboData) return;

	// Check if sequence is complete
	if (!ComboData->RageSequence.IsValidIndex(CurrentStageIndex))
	{
		if (ActiveRageEmitter)
		{
			ActiveRageEmitter->DestroyComponent();
			ActiveRageEmitter = nullptr;
		}
		EnterRageMode();
		return;
	}

	const FDMC_RageStage& CurrentStage = ComboData->RageSequence[CurrentStageIndex];

	// Handle Visual Effects
	if (CurrentStage.StageFX)
	{
		if (CurrentStage.bDestroyPreviousFX && ActiveRageEmitter)
		{
			ActiveRageEmitter->DestroyComponent();
			ActiveRageEmitter = nullptr;
		}

		UParticleSystemComponent* NewEmitter = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			CurrentStage.StageFX,
			Player->GetActorLocation(),
			Player->GetActorRotation(),
			CurrentStage.FXScale
		);

		if (!CurrentStage.bDestroyPreviousFX)
		{
			ActiveRageEmitter = NewEmitter;
		}
	}

	// Handle Material Feedback
	if (CurrentStage.OverlayMaterial && Player->GetMesh())
	{
		Player->GetMesh()->SetOverlayMaterial(CurrentStage.OverlayMaterial);
	}

	// Transition to next stage after defined delay
	CurrentStageIndex++;
	GetWorld()->GetTimerManager().SetTimer(RageTimerHandle, this, &UDMC_RageComponent::ExecuteNextRageStage, CurrentStage.DelayToNextStage, false);
}

void UDMC_RageComponent::EnterRageMode()
{
	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	if (!Player) return;

	UDMC_ComboDataAsset* ComboData = Player->GetComboData();
	if (!ComboData) return;

	// Apply combat buffs
	Player->CustomTimeDilation = ComboData->RageTimeDilation;
	Player->SetKatanaDamage(ComboData->RageDamageMultiplier);
	bRageActive = true;

	// Clear combo state to allow a fresh start in Rage
	Player->SetState(EDMC_PlayerState::ECS_Nothing);
	Player->ResetLightAttackVariables();
	Player->ResetHeavyAttackVariables();

	GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, this, &UDMC_RageComponent::StopRage, ComboData->RageDuration, false);
}
// ~ End Internal Implementation
