#include "PlayerCharacter/Notifies/Notify_LaunchCharacter.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"

void UNotify_LaunchCharacter::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (ADMC_PlayerCharacter* PlayerCharacter = Cast<ADMC_PlayerCharacter>(MeshComp->GetOwner()))
	{
		PlayerCharacter->LaunchCharacterUp();
	}
}