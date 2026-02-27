#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Notify_EnemyFire.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UNotify_EnemyFire : public UAnimNotify
{
	GENERATED_BODY()

public:
	// ~ Begin UAnimNotify interface
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference	
	) override;
	// ~ End UAnimNotify interface
};