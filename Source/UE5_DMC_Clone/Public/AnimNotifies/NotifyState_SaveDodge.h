#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NotifyState_SaveDodge.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UNotifyState_SaveDodge : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	// ~ Begin UAnimNotifyState interface
	virtual void NotifyTick(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float FrameDeltaTime,
		const FAnimNotifyEventReference& EventReference	
	) override;
	// ~ End UAnimNotifyState interface
};