#include "AnimNotifies/NotifyState_SaveDodge.h"
#include "Interfaces/DMC_CombatInterface.h"

void UNotifyState_SaveDodge::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (IDMC_CombatInterface* CombatInterface = Cast<IDMC_CombatInterface>(MeshComp->GetOwner()))
	{
		CombatInterface->SaveDodge();
	}
}