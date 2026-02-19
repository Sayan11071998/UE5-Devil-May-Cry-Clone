#include "Components/DMC_CombatComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"
#include "Data/DMC_ComboDataAsset.h"
#include "Components/DMC_CombatBufferComponent.h"
#include "Components/DMC_RageComponent.h"
#include "Components/DMC_TargetingComponent.h"
#include "Data/DMC_ComboDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"

UDMC_CombatComponent::UDMC_CombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDMC_CombatComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerOwner = Cast<ADMC_PlayerCharacter>(GetOwner());
}

void UDMC_CombatComponent::PerformLightAttack()
{
	if (!PlayerOwner) return;

	if (PlayerOwner->IsBusy())
	{
		if (UDMC_CombatBufferComponent* Buffer = PlayerOwner->GetBufferComponent())
		{
			Buffer->BufferInput(EDMC_BufferedInput::EBI_LightAttack);
		}
	}
	else
	{
		// Special Attack logic (Stinger, etc.) still handled by character for now via delegated calls
		if (!PlayerOwner->GetCharacterMovement()->IsFalling())
		{
			ResetHeavyCombo();
			Internal_PerformLightAttack(ComboState.LightIndex);
		}
	}
}

void UDMC_CombatComponent::PerformHeavyAttack()
{
	if (!PlayerOwner) return;

	if (PlayerOwner->IsBusy())
	{
		if (UDMC_CombatBufferComponent* Buffer = PlayerOwner->GetBufferComponent())
		{
			Buffer->BufferInput(EDMC_BufferedInput::EBI_HeavyAttack);
		}
	}
	else
	{
		if (!PlayerOwner->GetCharacterMovement()->IsFalling())
		{
			ResetLightCombo();
			Internal_PerformHeavyAttack(ComboState.HeavyIndex);
		}
	}
}

void UDMC_CombatComponent::PerformDodge()
{
	if (!PlayerOwner) return;

	PlayerOwner->StopRotation();
	bPerformChargeAttack = false;
	if (UDMC_TargetingComponent* Targeting = PlayerOwner->GetTargetingComp())
	{
		Targeting->ClearSoftTarget();
	}
	
	FVector LastInput = PlayerOwner->GetCharacterMovement()->GetLastInputVector();
	if (!LastInput.IsNearlyZero())
	{
		PlayerOwner->SetActorRotation(LastInput.Rotation());
	}
	
	if (UDMC_CombatBufferComponent* Buffer = PlayerOwner->GetBufferComponent())
	{
		Buffer->StopBuffer();
	}
	
	UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
	if (ComboData)
	{
		if (UDMC_CombatBufferComponent* Buffer = PlayerOwner->GetBufferComponent())
		{
			Buffer->StartBuffer(ComboData->DodgeBufferAmount);
		}
		
		PlayerOwner->SetState(EDMC_PlayerState::ECS_Dodge);
		
		if (ComboData->DodgeMontage)
		{
			PlayerOwner->PlayAnimMontage(ComboData->DodgeMontage);
		}
	}
}

void UDMC_CombatComponent::SpecialAttack()
{
	UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
	if (!ComboData || ComboData->SpecialAttacks.Num() == 0) return;

	const FVector LastInput = PlayerOwner->GetCharacterMovement()->GetLastInputVector();
	const float ForwardDot = !LastInput.IsNearlyZero() ? FVector::DotProduct(PlayerOwner->GetActorForwardVector(), LastInput.GetSafeNormal()) : 0.f;
	const bool bIsFalling = PlayerOwner->GetCharacterMovement()->IsFalling();
	const bool bHasTarget = PlayerOwner->GetIsTargeting() && PlayerOwner->GetCombatTarget() != nullptr;

	for (const FDMC_SpecialAttackData& AttackData : ComboData->SpecialAttacks)
	{
		if (!AttackData.Montage) continue;

		// 1. Check Flags
		if (AttackData.bCheckDodgeFlag && !bDodgeAttackEnabled) continue;
		if (AttackData.bCheckChargeFlag && !bPerformChargeAttack) continue;

		// 2. Check Direction (if enabled)
		if (AttackData.MinForwardDot > -1.05f || AttackData.MaxForwardDot < 1.05f)
		{
			if (LastInput.IsNearlyZero() || ForwardDot < AttackData.MinForwardDot || ForwardDot > AttackData.MaxForwardDot)
			{
				continue;
			}
		}

		// 3. Check Requirements
		bool bReqsMet = true;
		for (EDMC_SpecialAttackRequirement Req : AttackData.Requirements)
		{
			switch (Req)
			{
			case EDMC_SpecialAttackRequirement::ESAR_RequiresTarget:
				if (!bHasTarget) bReqsMet = false;
				break;
			case EDMC_SpecialAttackRequirement::ESAR_RequiresNoTarget:
				if (bHasTarget) bReqsMet = false;
				break;
			case EDMC_SpecialAttackRequirement::ESAR_GroundOnly:
				if (bIsFalling) bReqsMet = false;
				break;
			case EDMC_SpecialAttackRequirement::ESAR_AirOnly:
				if (!bIsFalling) bReqsMet = false;
				break;
			default: ;
			}
			if (!bReqsMet) break;
		}

		if (!bReqsMet) continue;

		// If we reached here, all conditions are met!
		if (ExecuteAttack(AttackData.Montage, AttackData.BufferAmount))
		{
			PlayerOwner->ResetLightAttackVariables();
			PlayerOwner->ResetHeavyAttackVariables();
			PlayerOwner->RotateToTarget();
			return;
		}
	}
}

bool UDMC_CombatComponent::ExecuteAttack(UAnimMontage* Montage, float BufferAmount)
{
	if (!PlayerOwner || !Montage) return false;

	if (UDMC_CombatBufferComponent* Buffer = PlayerOwner->GetBufferComponent())
	{
		Buffer->StopBuffer();
		Buffer->StartBuffer(BufferAmount);
	}

	PlayerOwner->SetState(EDMC_PlayerState::ECS_Attack);
	PlayerOwner->SoftLockOn();
	PlayerOwner->PlayAnimMontage(Montage);

	return true;
}

bool UDMC_CombatComponent::Internal_PerformLightAttack(int32 InAttackIndex)
{
	UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
	if (!ComboData) return false;

	const bool bIsRageActive = PlayerOwner->IsRaging();
	const TArray<TObjectPtr<UAnimMontage>>& CurrentCombo = bIsRageActive ? ComboData->LightAttackRageCombo : ComboData->LightAttackCombo;

	if (!CurrentCombo.IsValidIndex(InAttackIndex)) return false;

	if (ExecuteAttack(CurrentCombo[InAttackIndex], ComboData->LightAttackBuffer))
	{
		ComboState.LightIndex++;
		if (ComboState.LightIndex >= CurrentCombo.Num())
		{
			ComboState.LightIndex = 0;
		}
		return true;
	}
	
	return false;
}

bool UDMC_CombatComponent::Internal_PerformHeavyAttack(int32 InAttackIndex)
{
	UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
	if (!ComboData || !ComboData->HeavyAttackCombo.IsValidIndex(InAttackIndex)) return false;

	if (ExecuteAttack(ComboData->HeavyAttackCombo[InAttackIndex], ComboData->HeavyAttackBuffer))
	{
		ComboState.HeavyIndex++;
		if (ComboState.HeavyIndex >= ComboData->HeavyAttackCombo.Num())
		{
			ComboState.HeavyIndex = 0;
		}
		return true;
	}
	
	return false;
}

bool UDMC_CombatComponent::Internal_PerformComboStarter()
{
	UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
	if (!ComboData || PlayerOwner->IsBusy() || PlayerOwner->GetCharacterMovement()->IsFalling()) return false;
	
	int32 HL_ComboStarterIndex = ComboState.HeavyIndex - 1;
	
	if (ComboData->ComboStarterMontages.IsValidIndex(HL_ComboStarterIndex))
	{
		if (ExecuteAttack(ComboData->ComboStarterMontages[HL_ComboStarterIndex], ComboData->StarterAttackBuffer))
		{
			ComboState.ExtenderIndex = ComboState.HeavyIndex;
			ResetHeavyCombo();
			return true;
		}
	}
	
	return false;
}

bool UDMC_CombatComponent::Internal_PerformComboExtender()
{
	UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
	if (!ComboData || PlayerOwner->IsBusy() || PlayerOwner->GetCharacterMovement()->IsFalling()) return false;

	int32 LH_FinisherIndex = ComboState.ExtenderIndex - 1;
	
	if (ComboData->ComboExtenderMontages.IsValidIndex(LH_FinisherIndex))
	{
		if (ExecuteAttack(ComboData->ComboExtenderMontages[LH_FinisherIndex], ComboData->ExtenderAttackBuffer))
		{
			ComboState.ExtenderIndex = 0;
			return true;
		}
	}
	
	return false;
}

void UDMC_CombatComponent::TryConsumeBufferedInput()
{
	if (!PlayerOwner) return;
	UDMC_CombatBufferComponent* Buffer = PlayerOwner->GetBufferComponent();
	if (!Buffer || !Buffer->HasBufferedInput()) return;

	EDMC_BufferedInput Input = Buffer->PopInput();
	PlayerOwner->SetState(EDMC_PlayerState::ECS_Nothing);

	switch (Input)
	{
	case EDMC_BufferedInput::EBI_Dodge:
		PerformDodge();
		break;
	case EDMC_BufferedInput::EBI_LightAttack:
		if (ComboState.HeavyIndex > 0)
		{
			Internal_PerformComboStarter();
		}
		else
		{
			PerformLightAttack();
		}
		break;
	case EDMC_BufferedInput::EBI_HeavyAttack:
		if (ComboState.ExtenderIndex > 0)
		{
			Internal_PerformComboExtender();
		}
		else
		{
			PerformHeavyAttack();
		}
		break;
	default:
		break;
	}
}

