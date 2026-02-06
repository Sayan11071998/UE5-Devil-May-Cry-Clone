#include "PlayerCharacter/Notifies/NotifyState_WeaponCollision.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"

void UNotifyState_WeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ADMC_PlayerCharacter* PlayerCharacter = Cast<ADMC_PlayerCharacter>(MeshComp->GetOwner()))
		{
			PlayerCharacter->StartWeaponCollision();
		}
	}
}

void UNotifyState_WeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ADMC_PlayerCharacter* PlayerCharacter = Cast<ADMC_PlayerCharacter>(MeshComp->GetOwner()))
		{
			PlayerCharacter->EndWeaponCollision();
		}
	}
}