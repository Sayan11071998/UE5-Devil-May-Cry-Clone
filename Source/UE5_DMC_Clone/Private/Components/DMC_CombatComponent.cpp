#include "Components/DMC_CombatComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"
#include "Data/DMC_ComboDataAsset.h"
#include "Components/DMC_CombatBufferComponent.h"
#include "Components/DMC_RageComponent.h"
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
			Internal_PerformLightAttack(LightAttackIndex);
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
			Internal_PerformHeavyAttack(HeavyAttackIndex);
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
		LightAttackIndex++;
		if (LightAttackIndex >= CurrentCombo.Num())
		{
			LightAttackIndex = 0;
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
		HeavyAttackIndex++;
		if (HeavyAttackIndex >= ComboData->HeavyAttackCombo.Num())
		{
			HeavyAttackIndex = 0;
		}
		return true;
	}
	
	return false;
}

bool UDMC_CombatComponent::Internal_PerformComboStarter()
{
	UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
	if (!ComboData || PlayerOwner->IsBusy() || PlayerOwner->GetCharacterMovement()->IsFalling()) return false;
	
	int32 HL_ComboStarterIndex = HeavyAttackIndex - 1;
	
	if (ComboData->ComboStarterMontages.IsValidIndex(HL_ComboStarterIndex))
	{
		if (ExecuteAttack(ComboData->ComboStarterMontages[HL_ComboStarterIndex], ComboData->StarterAttackBuffer))
		{
			ComboExtenderIndex = HeavyAttackIndex;
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

	int32 LH_FinisherIndex = ComboExtenderIndex - 1;
	
	if (ComboData->ComboExtenderMontages.IsValidIndex(LH_FinisherIndex))
	{
		if (ExecuteAttack(ComboData->ComboExtenderMontages[LH_FinisherIndex], ComboData->ExtenderAttackBuffer))
		{
			ComboExtenderIndex = 0;
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
		PlayerOwner->Dodge(); // Dodge still handled by character
		break;
	case EDMC_BufferedInput::EBI_LightAttack:
		if (HeavyAttackIndex > 0)
		{
			Internal_PerformComboStarter();
		}
		else
		{
			PerformLightAttack();
		}
		break;
	case EDMC_BufferedInput::EBI_HeavyAttack:
		if (ComboExtenderIndex > 0)
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

void UDMC_CombatComponent::ResetLightCombo()
{
	LightAttackIndex = 0;
}

void UDMC_CombatComponent::ResetHeavyCombo()
{
	HeavyAttackIndex = 0;
}
