#include "PlayerCharacter/Notifies/Notify_ResetState.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"

void UNotify_ResetState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ADMC_PlayerCharacter* PlayerCharacter = Cast<ADMC_PlayerCharacter>(MeshComp->GetOwner()))
		{
			PlayerCharacter->ResetState();
		}
	}
}