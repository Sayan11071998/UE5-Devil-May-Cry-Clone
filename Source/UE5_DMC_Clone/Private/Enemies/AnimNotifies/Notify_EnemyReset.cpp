#include "Enemies/AnimNotifies/Notify_EnemyReset.h"

#include "Enemies/DMC_EnemyCharacterBase.h"

void UNotify_EnemyReset::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (ADMC_EnemyCharacterBase* EnemyCharacter = Cast<ADMC_EnemyCharacterBase>(MeshComp->GetOwner()))
	{
		EnemyCharacter->EnemyReset();
	}
}