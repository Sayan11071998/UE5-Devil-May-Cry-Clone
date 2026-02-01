#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DMC_PlayerAnimInstance.generated.h"

enum class EDMC_PlayerState : uint8;
class UCharacterMovementComponent;
class ADMC_PlayerCharacter;

UCLASS()
class UE5_DMC_CLONE_API UDMC_PlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// ~ Begin UAnimInstance interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	// ~ End UAnimInstance interface
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<ADMC_PlayerCharacter> PlayerCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<UCharacterMovementComponent> PlayerCharacterMovement;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Speed;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;
	
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EDMC_PlayerState CurrentState;
};