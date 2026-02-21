#include "Components/DMC_CombatComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"
#include "Components/DMC_CombatBufferComponent.h"
#include "Components/DMC_TargetingComponent.h"
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
	else if (!PlayerOwner->GetCharacterMovement()->IsFalling())
	{
		ResetHeavyCombo();
		Internal_ExecuteComboStep(PlayerOwner->IsRaging() ? PlayerOwner->GetComboData()->LightAttackRageCombo : PlayerOwner->GetComboData()->LightAttackCombo, ComboState.LightIndex);
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
	else if (!PlayerOwner->GetCharacterMovement()->IsFalling())
	{
		ResetLightCombo();
		Internal_ExecuteComboStep(PlayerOwner->GetComboData()->HeavyAttackCombo, ComboState.HeavyIndex);
	}
}

void UDMC_CombatComponent::PerformDodge()
{
	if (!PlayerOwner) return;

	PlayerOwner->StopRotation();
	bPerformChargeAttack = false;

	const FVector LastInput = PlayerOwner->GetCharacterMovement()->GetLastInputVector();
	if (!LastInput.IsNearlyZero())
	{
		PlayerOwner->SetActorRotation(LastInput.Rotation());
	}
	
	if (UDMC_CombatBufferComponent* Buffer = PlayerOwner->GetBufferComponent())
	{
		Buffer->StopBuffer();
	}
	
	if (UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData())
	{
		if (UDMC_CombatBufferComponent* Buffer = PlayerOwner->GetBufferComponent())
		{
			Buffer->StartBuffer(ComboData->DodgeData.BufferAmount);
		}
		
		PlayerOwner->SetState(EDMC_PlayerState::ECS_Dodge);
		if (ComboData->DodgeData.Montage)
		{
			PlayerOwner->PlayAnimMontage(ComboData->DodgeData.Montage);
		}
	}
}

bool UDMC_CombatComponent::SpecialAttack()
{
	UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
	if (!ComboData || ComboData->SpecialAttacks.Num() == 0) return false;

	const FVector LastInput = PlayerOwner->GetCharacterMovement()->GetLastInputVector();
	const float ForwardDot = !LastInput.IsNearlyZero() ? FVector::DotProduct(PlayerOwner->GetActorForwardVector(), LastInput.GetSafeNormal()) : 0.f;
	const bool bIsFalling = PlayerOwner->GetCharacterMovement()->IsFalling();
	const bool bHasTarget = PlayerOwner->GetCombatTarget() != nullptr;

	for (const FDMC_SpecialAttackData& AttackData : ComboData->SpecialAttacks)
	{
		if (!AttackData.Montage) continue;
		if (AttackData.bCheckDodgeFlag && !bDodgeAttackEnabled) continue;
		if (AttackData.bCheckChargeFlag && !bPerformChargeAttack) continue;

		// Check Directional Alignment
		if (AttackData.MinForwardDot > -1.05f || AttackData.MaxForwardDot < 1.05f)
		{
			if (LastInput.IsNearlyZero() || ForwardDot < AttackData.MinForwardDot || ForwardDot > AttackData.MaxForwardDot)
			{
				continue;
			}
		}
		
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
			
			case EDMC_SpecialAttackRequirement::ESAR_FinisherOnly:
				bReqsMet = false; // FinisherOnly is handled by FinisherComponent
				break;
				
			default:
				break;
			}
			
			if (!bReqsMet) break;
		}

		if (bReqsMet && ExecuteAttack(FDMC_AttackData{AttackData.Montage, AttackData.BufferAmount}))
		{
			PlayerOwner->ResetLightAttackVariables();
			PlayerOwner->ResetHeavyAttackVariables();
			if (UDMC_TargetingComponent* Targeting = PlayerOwner->GetTargetingComp())
			{
				Targeting->SnapToTarget();
			}
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

	const EDMC_BufferedInput Input = Buffer->PopInput();
	PlayerOwner->SetState(EDMC_PlayerState::ECS_Nothing);

	switch (Input)
	{
	case EDMC_BufferedInput::EBI_Dodge:
		PerformDodge();
		break;
		
	case EDMC_BufferedInput::EBI_LightAttack:
		if (ComboState.HeavyIndex > 0) Internal_PerformComboStarter();
		else PerformLightAttack();
		break;
		
	case EDMC_BufferedInput::EBI_HeavyAttack:
		if (ComboState.ExtenderIndex > 0) Internal_PerformComboExtender();
		else PerformHeavyAttack();
		break;
		
	default:
		break;
	}
}

bool UDMC_CombatComponent::ExecuteAttack(const FDMC_AttackData& AttackData)
{
	if (!PlayerOwner || !AttackData.Montage) return false;

	if (UDMC_CombatBufferComponent* Buffer = PlayerOwner->GetBufferComponent())
	{
		Buffer->StopBuffer();
		Buffer->StartBuffer(AttackData.BufferAmount);
	}

	PlayerOwner->SetState(EDMC_PlayerState::ECS_Attack);
	if (UDMC_TargetingComponent* Targeting = PlayerOwner->GetTargetingComp())
	{
		Targeting->SnapToTarget();
	}
	PlayerOwner->PlayAnimMontage(AttackData.Montage);

	return true;
}

bool UDMC_CombatComponent::Internal_ExecuteComboStep(const TArray<FDMC_AttackData>& ComboArray, int32& OutIndex)
{
	if (!ComboArray.IsValidIndex(OutIndex)) return false;

	if (ExecuteAttack(ComboArray[OutIndex]))
	{
		OutIndex = (OutIndex + 1) % ComboArray.Num();
		return true;
	}
	return false;
}

bool UDMC_CombatComponent::Internal_PerformComboStarter()
{
	UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
	if (!ComboData || PlayerOwner->IsBusy() || PlayerOwner->GetCharacterMovement()->IsFalling()) return false;
	
	const int32 HL_ComboStarterIndex = ComboState.HeavyIndex - 1;
	if (ComboData->ComboStarterMontages.IsValidIndex(HL_ComboStarterIndex))
	{
		if (ExecuteAttack(ComboData->ComboStarterMontages[HL_ComboStarterIndex]))
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

	const int32 LH_FinisherIndex = ComboState.ExtenderIndex - 1;
	if (ComboData->ComboExtenderMontages.IsValidIndex(LH_FinisherIndex))
	{
		if (ExecuteAttack(ComboData->ComboExtenderMontages[LH_FinisherIndex]))
		{
			ComboState.ExtenderIndex = 0;
			return true;
		}
	}
	return false;
}