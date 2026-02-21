#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Notify_RotationToTarget.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UNotify_RotationToTarget : public UAnimNotify
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