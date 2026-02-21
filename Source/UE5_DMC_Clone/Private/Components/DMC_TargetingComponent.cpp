#include "Components/DMC_TargetingComponent.h"
#include "Camera/CameraComponent.h"
#include "Enemies/DMC_EnemyCharacterBase.h"
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
	
	SoftLockOn();

	if (RotationTimeline.IsPlaying())
	{
		RotationTimeline.TickTimeline(DeltaTime);
	}
}

void UDMC_TargetingComponent::SoftLockOn()
{
	FocusedTarget = FindBestContextualTarget();
}

AActor* UDMC_TargetingComponent::FindBestContextualTarget()
{
	ADMC_PlayerCharacter* Player = Cast<ADMC_PlayerCharacter>(GetOwner());
	if (!Player) return nullptr;

	FVector Start = Player->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Player);

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Start, MaxSearchRadius, ObjectTypes, ADMC_EnemyCharacterBase::StaticClass(), ActorsToIgnore, OutActors);

	AActor* BestTarget = nullptr;
	float BestScore = -1.f;

	FVector CameraLocation = Player->GetFollowCamera()->GetComponentLocation();
	FVector CameraForward = Player->GetFollowCamera()->GetForwardVector();
	FVector PlayerInput = Player->GetCharacterMovement()->GetLastInputVector().GetSafeNormal();

	for (AActor* Actor : OutActors)
	{
		ADMC_EnemyCharacterBase* Enemy = Cast<ADMC_EnemyCharacterBase>(Actor);
		if (!Enemy || Enemy->IsDead()) continue;

		FVector ToEnemy = (Enemy->GetActorLocation() - Start).GetSafeNormal();
		
		// Angle from Camera Forward (Center of Screen)
		float DotToCamera = FVector::DotProduct(CameraForward, ToEnemy);
		float AngleDegree = FMath::RadiansToDegrees(FMath::Acos(DotToCamera));
		
		if (AngleDegree > MaxFieldOfViewAngle) continue; // Outside of "Combat FOV"

		// Proximity Scoring (Normalizing distance)
		float Distance = Player->GetDistanceTo(Enemy);
		float DistanceScore = 1.0f - FMath::Clamp(Distance / MaxSearchRadius, 0.f, 1.f);

		// Input Bias
		float InputScore = 0.f;
		if (!PlayerInput.IsNearlyZero())
		{
			InputScore = FMath::Max(0.f, FVector::DotProduct(PlayerInput, ToEnemy));
		}

		// Combined Score (Weights)
		// Favoring Center of Screen (DotToCamera) and Proximity
		float TotalScore = (DotToCamera * 0.5f) + (DistanceScore * 0.3f) + (InputScore * 0.2f);

		if (TotalScore > BestScore)
		{
			BestScore = TotalScore;
			BestTarget = Enemy;
		}
	}

	return BestTarget;
}

void UDMC_TargetingComponent::RotateToTarget()
{
	if (IsValid(FocusedTarget) && RotationCurve)
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
	FocusedTarget = nullptr;
}

void UDMC_TargetingComponent::HandleRotationTimelineProgress(float Value)
{
	AActor* Owner = GetOwner();
	if (!Owner || !IsValid(FocusedTarget))
	{
		StopRotation();
		return;
	}
	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), FocusedTarget->GetActorLocation());
	FRotator CurrentRot = Owner->GetActorRotation();
	FRotator TargetRot = FRotator(CurrentRot.Pitch, LookAtRot.Yaw, CurrentRot.Roll);
	Owner->SetActorRotation(FMath::Lerp(CurrentRot, TargetRot, Value));
}