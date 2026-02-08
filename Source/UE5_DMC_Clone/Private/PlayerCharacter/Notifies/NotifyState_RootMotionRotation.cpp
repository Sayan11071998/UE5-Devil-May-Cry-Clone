#include "PlayerCharacter/Notifies/NotifyState_RootMotionRotation.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"

void UNotifyState_RootMotionRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ADMC_PlayerCharacter* PlayerCharacter = Cast<ADMC_PlayerCharacter>(MeshComp->GetOwner()))
		{
			bool bAllowPhysicsRotation = !IsValid(PlayerCharacter->GetSoftTarget()) || !IsValid(PlayerCharacter->GetTargetActor());
			
			if (bAllowPhysicsRotation)
			{
				PlayerCharacter->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
			}
		}
	}
}

void UNotifyState_RootMotionRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ADMC_PlayerCharacter* PlayerCharacter = Cast<ADMC_PlayerCharacter>(MeshComp->GetOwner()))
		{
			PlayerCharacter->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
		}
	}
}