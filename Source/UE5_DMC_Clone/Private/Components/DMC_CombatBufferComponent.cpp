#include "Components/DMC_CombatBufferComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UDMC_CombatBufferComponent::UDMC_CombatBufferComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UDMC_CombatBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bIsBuffering || !BufferCurve) return;
	
	BufferTimeElapsed += DeltaTime;
		
	if (BufferTimeElapsed <= BufferDuration)
	{
		float Alpha = BufferCurve->GetFloatValue(BufferTimeElapsed);
		
		AActor* Owner = GetOwner();
		
		FVector CurrentLoc = Owner->GetActorLocation();
		FVector Forward = Owner->GetActorForwardVector();
		FVector TargetLoc = CurrentLoc + (Forward * CurrentBufferAmount);
		
		// The 60.f ensures movement feels the same regardless of frame rate
		FVector NewLoc = FMath::Lerp(CurrentLoc, TargetLoc, Alpha * DeltaTime * 60.f);

		Owner->SetActorLocation(NewLoc, true);
	}
	else
	{
		StopBuffer();
	}
}

void UDMC_CombatBufferComponent::StartBuffer(float Amount)
{
	CurrentBufferAmount = Amount;
	BufferTimeElapsed = 0.f;
	bIsBuffering = true;
	
	if (AActor* Owner = GetOwner())
	{
		if (ACharacter* Character = Cast<ACharacter>(Owner))
		{
			Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
			Character->GetCharacterMovement()->StopMovementImmediately();
		}
	}

	SetComponentTickEnabled(true);
}

void UDMC_CombatBufferComponent::StopBuffer()
{
	bIsBuffering = false;
	
	SetComponentTickEnabled(false);
}