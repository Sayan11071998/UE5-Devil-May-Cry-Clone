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

void UDMC_RageComponent::ExecuteNextRageStage()
{
	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	if (!Player) return;

	UDMC_ComboDataAsset* ComboData = Player->GetComboData();
	if (!ComboData) return;

	// Check if we've reached the end of the sequence
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

	// Handle FX
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

	// Handle Material Overlay
	if (CurrentStage.OverlayMaterial && Player->GetMesh())
	{
		Player->GetMesh()->SetOverlayMaterial(CurrentStage.OverlayMaterial);
	}

	// Move to next stage after delay
	CurrentStageIndex++;
	GetWorld()->GetTimerManager().SetTimer(RageTimerHandle, this, &UDMC_RageComponent::ExecuteNextRageStage, CurrentStage.DelayToNextStage, false);
}

void UDMC_RageComponent::EnterRageMode()
{
	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	if (!Player) return;

	UDMC_ComboDataAsset* ComboData = Player->GetComboData();
	if (!ComboData) return;

	Player->CustomTimeDilation = ComboData->RageTimeDilation;
	Player->SetKatanaDamage(ComboData->RageDamageMultiplier);
	bRageActive = true;

	Player->SetState(EDMC_PlayerState::ECS_Nothing);
	Player->ResetLightAttackVariables();
	Player->ResetHeavyAttackVariables();
	// Note: ComboExtenderIndex reset is handled in character logic or reset methods

	GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, this, &UDMC_RageComponent::StopRage, ComboData->RageDuration, false);
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
