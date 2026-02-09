#include "PlayerCharacter/Notifies/Notify_SaveHeavyAttack.h"
#include "Interfaces/DMC_CombatInterface.h"

void UNotify_SaveHeavyAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (IDMC_CombatInterface* CombatInterface = Cast<IDMC_CombatInterface>(MeshComp->GetOwner()))
	{
		CombatInterface->SaveHeavyAttack();
	}
}