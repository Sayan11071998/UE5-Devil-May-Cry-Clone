#include "AnimNotifies/Notify_EnemyFire.h"
#include "Enemies/DMC_EnemyRanged.h"

void UNotify_EnemyFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (ADMC_EnemyRanged* RangedEnemy = Cast<ADMC_EnemyRanged>(MeshComp->GetOwner()))
		{
			RangedEnemy->Fire();
		}
	}
}