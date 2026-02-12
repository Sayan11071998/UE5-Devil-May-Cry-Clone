#include "PlayerCharacter/DMC_PlayerAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"

void UDMC_PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	// Cache references to player character and movement component
	PlayerCharacter = Cast<ADMC_PlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter)
	{
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
	
	// Initialize animation variables
	Speed = 0.f;
	Direction = 0.f;
	bIsFalling = false;
	bDoubleJump = false;
	bIsTargeting = false;
	CurrentState = EDMC_PlayerState::ECS_Nothing;
}

void UDMC_PlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!PlayerCharacter || !PlayerCharacterMovement) return;
	
	FVector Velocity = PlayerCharacter->GetVelocity();
	Speed = Velocity.Size2D();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, PlayerCharacter->GetActorRotation());
	
	// Update state flags for the AnimGraph
	bIsFalling = PlayerCharacterMovement->IsFalling() || PlayerCharacterMovement->IsFlying();
	bDoubleJump = PlayerCharacter->GetDoubleJumpState();
	bIsTargeting = PlayerCharacter->GetIsTargeting();
	CurrentState = PlayerCharacter->GetState();
}