#include "Components/DMC_TargetingComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter/DMC_PlayerCharacter.h"

UDMC_TargetingComponent::UDMC_TargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDMC_TargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	
	if (!Player) return;
	
	if (bIsTargeting && IsValid(TargetActor))
	{
		TArray<EDMC_PlayerState> DodgeState;
		DodgeState.Add(EDMC_PlayerState::ECS_Dodge);
		
		if (!Player->IsStateEqualToAny(DodgeState))
		{
			Player->GetCharacterMovement()->bUseControllerDesiredRotation = true;
			Player->GetCharacterMovement()->bOrientRotationToMovement = false;
			
			if (AController* PlayerController = Player->GetController())
			{
				FRotator CurrentRotation = PlayerController->GetControlRotation(); 
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Player->GetActorLocation(), TargetActor->GetActorLocation());
				FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 5.0f);
				PlayerController->SetControlRotation(NewRotation);
			}
		}
		else
		{
			Player->GetCharacterMovement()->bUseControllerDesiredRotation = false;
			Player->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}
	
	if (RotationTimeline.IsPlaying())
	{
		RotationTimeline.TickTimeline(DeltaTime);
	}
}

void UDMC_TargetingComponent::LockOn()
{
	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	
	if (!Player) return;
	
	bInputHold = true;
	
	FVector Start = Player->GetActorLocation();
	FVector End = Start + (Player->GetFollowCamera()->GetForwardVector() * 1000.f);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
    
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Player);
	
	FHitResult OutHit;
	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		150.f,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true	
	);
	
	if (bHit && IsValid(OutHit.GetActor()))
	{
		bIsTargeting = true;
		TargetActor = OutHit.GetActor();
		Player->GetCharacterMovement()->MaxWalkSpeed = 250.f;
	}
	else
	{
		StopLockOn();
	}
}

void UDMC_TargetingComponent::StopLockOn()
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	
	if (Owner)
	{
		Owner->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		Owner->GetCharacterMovement()->bOrientRotationToMovement = true;
		Owner->GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	
	bInputHold = false;
	bIsTargeting = false;
	TargetActor = nullptr;
}

void UDMC_TargetingComponent::SoftLockOn()
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	
	if (!Owner || (bIsTargeting && IsValid(SoftTarget))) return;
	
	FVector LastInput = Owner->GetCharacterMovement()->GetLastInputVector();
	
	if (LastInput.IsNearlyZero()) return;
	
	FVector Start = Owner->GetActorLocation();
	FVector End = Start + (LastInput * 1000.f);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);
	
	FHitResult OutHit;
	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		100.f,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true	
	);
	
	SoftTarget = bHit ? OutHit.GetActor() : nullptr;
}

void UDMC_TargetingComponent::RotateToTarget()
{
	if ((IsValid(TargetActor) || IsValid(SoftTarget)) && RotationCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleRotationTimelineProgress"));
		RotationTimeline.AddInterpFloat(RotationCurve, ProgressFunction);
		RotationTimeline.SetLooping(false);
		RotationTimeline.PlayFromStart();
	}
}

void UDMC_TargetingComponent::StopRotation()
{
	if (RotationTimeline.IsPlaying())
	{
		RotationTimeline.Stop();
	}
}

void UDMC_TargetingComponent::ClearSoftTarget()
{
	SoftTarget = nullptr;
}

void UDMC_TargetingComponent::HandleRotationTimelineProgress(float Value)
{
	AActor* Owner = GetOwner();
	AActor* ActualTarget = IsValid(TargetActor) ? TargetActor.Get() : SoftTarget.Get();
	
	if (!Owner || !IsValid(ActualTarget))
	{
		StopRotation();
		return;
	}
	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), ActualTarget->GetActorLocation());
	FRotator CurrentRot = Owner->GetActorRotation();
	FRotator TargetRot = FRotator(CurrentRot.Pitch, LookAtRot.Yaw, CurrentRot.Roll);
	Owner->SetActorRotation(FMath::Lerp(CurrentRot, TargetRot, Value));
}