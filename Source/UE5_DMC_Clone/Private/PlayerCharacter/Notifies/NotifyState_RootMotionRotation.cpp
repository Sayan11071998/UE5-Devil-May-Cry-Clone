#include "PlayerCharacter/Notifies/NotifyState_RootMotionRotation.h"
#include "Interfaces/DMC_CombatInterface.h"

void UNotifyState_RootMotionRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (IDMC_CombatInterface* CombatInterface = Cast<IDMC_CombatInterface>(MeshComp->GetOwner()))
	{
		bool bAllowPhysicsRotation =
			!IsValid(CombatInterface->GetSoftTarget()) ||
			!IsValid(CombatInterface->GetCombatTarget());
			
		if (bAllowPhysicsRotation)
		{
			CombatInterface->SetAllowPhysicsRotation(true);
		}
	}
}

void UNotifyState_RootMotionRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (IDMC_CombatInterface* CombatInterface = Cast<IDMC_CombatInterface>(MeshComp->GetOwner()))
	{
		CombatInterface->SetAllowPhysicsRotation(false);
	}
}