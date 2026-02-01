#include "PlayerCharacter/DMC_PlayerAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"

void UDMC_PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	PlayerCharacter = Cast<ADMC_PlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter)
	{
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
	
	Speed = 0.f;
	bIsFalling = false;
	CurrentState = EDMC_PlayerState::ECS_Nothing;
}

void UDMC_PlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// Re-cache if references are invalid (edge case handling)
	if (!PlayerCharacter || !PlayerCharacterMovement)
	{
		PlayerCharacter = Cast<ADMC_PlayerCharacter>(TryGetPawnOwner());
		if (PlayerCharacter)
		{
			PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
		}
	}
	
	if (PlayerCharacter && PlayerCharacterMovement)
	{
		FVector Velocity = PlayerCharacter->GetVelocity();
		
		Speed = Velocity.Size();
		bIsFalling = PlayerCharacterMovement->IsFalling();
		CurrentState = PlayerCharacter->GetState();
	}
}