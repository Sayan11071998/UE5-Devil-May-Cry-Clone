#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NotifyState_WeaponCollision.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UNotifyState_WeaponCollision : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	// ~ Begin UAnimNotifyState interface
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference	
	) override;
	
	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
	// ~ End UAnimNotifyState interface
};