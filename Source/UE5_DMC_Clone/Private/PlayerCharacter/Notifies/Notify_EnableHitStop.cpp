#include "PlayerCharacter/Notifies/Notify_EnableHitStop.h"
#include "Interfaces/DMC_CombatInterface.h"

void UNotify_EnableHitStop::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (IDMC_CombatInterface* CombatInterface = Cast<IDMC_CombatInterface>(MeshComp->GetOwner()))
		{
			CombatInterface->EnableHitStop(bEnableHitStop);
		}
	}
}