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

		if (ComboData->RageParticles_1)
		{
			ActiveRageEmitter = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ComboData->RageParticles_1,
				Player->GetActorLocation(), // Use Actor Location as fallback
				Player->GetActorRotation(),
				FVector(1.6f)
			);
		}

		GetWorld()->GetTimerManager().SetTimer(RageTimerHandle, this, &UDMC_RageComponent::RageStage2, 1.0f, false);
	}
}

void UDMC_RageComponent::RageStage2()
{
	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	if (!Player) return;

	UDMC_ComboDataAsset* ComboData = Player->GetComboData();
	if (ComboData && ComboData->RageParticles_2)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ComboData->RageParticles_2,
			Player->GetActorLocation(),
			Player->GetActorRotation(),
			FVector(1.0f)
		);
	}

	GetWorld()->GetTimerManager().SetTimer(RageTimerHandle, this, &UDMC_RageComponent::RageStage3, 0.4f, false);
}

void UDMC_RageComponent::RageStage3()
{
	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	if (!Player) return;

	UDMC_ComboDataAsset* ComboData = Player->GetComboData();
	if (ComboData && ComboData->RageOverlayMaterial)
	{
		Player->GetMesh()->SetOverlayMaterial(ComboData->RageOverlayMaterial);
	}

	GetWorld()->GetTimerManager().SetTimer(RageTimerHandle, this, &UDMC_RageComponent::RageStage4, 0.1f, false);
}

void UDMC_RageComponent::RageStage4()
{
	if (ActiveRageEmitter)
	{
		ActiveRageEmitter->DestroyComponent();
		ActiveRageEmitter = nullptr;
	}

	EnterRageMode();
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
