#include "Components/DMC_FinisherComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"
#include "Interfaces/DMC_CombatInterface.h"
#include "Components/DMC_TargetingComponent.h"
#include "Components/DMC_CombatBufferComponent.h"
#include "Data/DMC_ComboDataAsset.h"
#include "Kismet/KismetMathLibrary.h"

UDMC_FinisherComponent::UDMC_FinisherComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDMC_FinisherComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerOwner = Cast<ADMC_PlayerCharacter>(GetOwner());
}

void UDMC_FinisherComponent::TryExecuteFinisher()
{
	if (!PlayerOwner) return;

	TArray<EDMC_PlayerState> StatesToIgnore;
	StatesToIgnore.Add(EDMC_PlayerState::ECS_Dodge);
	StatesToIgnore.Add(EDMC_PlayerState::ECS_Finisher);
	
	if (PlayerOwner->IsStateEqualToAny(StatesToIgnore))
	{
		UE_LOG(LogTemp, Warning, TEXT("[FinisherComp] TryExecuteFinisher ignored: Player is in an invalid state."));
		return;
	}

	// Support both Hard and Soft targets
	AActor* Target = PlayerOwner->GetCombatTarget() ? PlayerOwner->GetCombatTarget() : PlayerOwner->GetSoftTarget();

	if (Target)
	{
		UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
		if (!ComboData)
		{
			UE_LOG(LogTemp, Error, TEXT("[FinisherComp] TryExecuteFinisher failed: No ComboData found."));
			return;
		}

		float Distance = PlayerOwner->GetDistanceTo(Target);
		if (Distance <= ComboData->FinisherAttackDistance)
		{
			if (IDMC_CombatInterface* CombatInterface = Cast<IDMC_CombatInterface>(Target))
			{
				if (!CombatInterface->CanBeFinished())
				{
					UE_LOG(LogTemp, Warning, TEXT("[FinisherComp] TryExecuteFinisher failed: Target cannot be finished yet."));
					return;
				}

				if (UDMC_TargetingComponent* TargetingComp = PlayerOwner->GetTargetingComp())
				{
					TargetingComp->StopRotation();
				}
				
				if (UDMC_CombatBufferComponent* BufferComponent = PlayerOwner->GetBufferComponent())
				{
					BufferComponent->StopBuffer();
				}
				
				CombatInterface->OnFinished(PlayerOwner);

				FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(PlayerOwner->GetActorLocation(), Target->GetActorLocation());
				PlayerOwner->SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f));
				PlayerOwner->SetState(EDMC_PlayerState::ECS_Finisher);
				
				UAnimMontage* FinisherMontage = nullptr;
				for (const FDMC_SpecialAttackData& AttackData : ComboData->SpecialAttacks)
				{
					if (AttackData.Requirements.Contains(EDMC_SpecialAttackRequirement::ESAR_FinisherOnly))
					{
						FinisherMontage = AttackData.Montage;
						break;
					}
				}

				if (FinisherMontage)
				{
					PlayerOwner->PlayAnimMontage(FinisherMontage);
					UE_LOG(LogTemp, Log, TEXT("[FinisherComp] Executing Finisher Montage."));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[FinisherComp] Finisher Montage not found in ComboData!"));
				}

				if (UDMC_TargetingComponent* TargetingComp = PlayerOwner->GetTargetingComp())
				{
					TargetingComp->StopLockOn();
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[FinisherComp] TryExecuteFinisher failed: Target too far (Distance: %f, Req: %f)"), Distance, ComboData->FinisherAttackDistance);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[FinisherComp] TryExecuteFinisher failed: No valid Target found (Hard or Soft)."));
	}
}