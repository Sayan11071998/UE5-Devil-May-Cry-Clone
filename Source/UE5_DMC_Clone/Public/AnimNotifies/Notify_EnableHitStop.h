#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Notify_EnableHitStop.generated.h"

UCLASS()
class UE5_DMC_CLONE_API UNotify_EnableHitStop : public UAnimNotify
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Stop")
	bool bEnableHitStop = true;
};