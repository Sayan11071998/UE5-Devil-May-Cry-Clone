#include "Components/DMC_FinisherComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"
#include "Enemies/DMC_EnemyCharacterBase.h"
#include "Components/DMC_TargetingComponent.h"
#include "Components/DMC_CombatBufferComponent.h"
#include "Data/DMC_ComboDataAsset.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	
	if (PlayerOwner->IsStateEqualToAny(StatesToIgnore)) return;

	if (PlayerOwner->GetIsTargeting() && PlayerOwner->GetTargetActor())
	{
		AActor* Target = PlayerOwner->GetTargetActor();
		UDMC_ComboDataAsset* ComboData = PlayerOwner->GetComboData();
		
		float Distance = PlayerOwner->GetDistanceTo(Target);
		if (ComboData && Distance <= ComboData->FinisherAttackDistance)
		{
			if (ADMC_EnemyCharacterBase* Enemy = Cast<ADMC_EnemyCharacterBase>(Target))
			{
				if (Enemy->GetHealth() / Enemy->GetMaxHealth() > 0.1f) return;

				if (UDMC_TargetingComponent* TargetingComp = PlayerOwner->GetTargetingComp())
				{
					TargetingComp->StopRotation();
				}
				if (UDMC_CombatBufferComponent* BufferComponent = PlayerOwner->GetBufferComponent())
				{
					BufferComponent->StopBuffer();
				}
				
				Enemy->Finished(PlayerOwner);

				FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(PlayerOwner->GetActorLocation(), Target->GetActorLocation());
				PlayerOwner->SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f));
				PlayerOwner->SetState(EDMC_PlayerState::ECS_Finisher);
				
				if (ComboData->FinisherAttackMontage)
				{
					PlayerOwner->PlayAnimMontage(ComboData->FinisherAttackMontage);
				}

				if (UDMC_TargetingComponent* TargetingComp = PlayerOwner->GetTargetingComp())
				{
					TargetingComp->StopLockOn();
				}
			}
		}
	}
}
