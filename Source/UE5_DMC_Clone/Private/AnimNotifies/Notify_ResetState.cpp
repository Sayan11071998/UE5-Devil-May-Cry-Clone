#include "AnimNotifies/Notify_ResetState.h"
#include "Interfaces/DMC_CombatInterface.h"

void UNotify_ResetState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (IDMC_CombatInterface* CombatInterface = Cast<IDMC_CombatInterface>(MeshComp->GetOwner()))
	{
		CombatInterface->ResetState();
	}
}