#include "Enemies/DMC_EnemyCharacterBase.h"
#include "DamageTypes/DMC_DamageType.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DMC_CombatBufferComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ADMC_EnemyCharacterBase::ADMC_EnemyCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BufferComponent = CreateDefaultSubobject<UDMC_CombatBufferComponent>(TEXT("CombatBuffer"));
}

void ADMC_EnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (LaunchCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleLaunchTimelineProgress"));
		LaunchTimeline.AddInterpFloat(LaunchCurve, ProgressFunction);
		LaunchTimeline.SetLooping(false);
	}
}

void ADMC_EnemyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (LaunchTimeline.IsPlaying())
	{
		LaunchTimeline.TickTimeline(DeltaTime);
	}
}

float ADMC_EnemyCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (DamageCauser)
	{
		FRotator CurrentRotation = GetActorRotation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DamageCauser->GetActorLocation());
		FRotator NewRotation = FRotator(CurrentRotation.Pitch, LookAtRotation.Yaw, CurrentRotation.Roll);
		SetActorRotation(NewRotation);
	}
	
	if (DamageEvent.DamageTypeClass)
	{
		UDMC_DamageType* DamageTypeObject = Cast<UDMC_DamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
		if (DamageTypeObject)
		{
			PlayHitReaction(DamageTypeObject->DamageType);
		}
	}
	
	return ActualDamage;
}

void ADMC_EnemyCharacterBase::EnemyReset()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void ADMC_EnemyCharacterBase::PlayHitReaction(EDMC_DamageType DamageDirection)
{
	if (DamageDirection == EDMC_DamageType::EDT_LaunchAttack)
	{
		const FDMC_HitReactionData& Data = HitReactionMap[DamageDirection];
		if (Data.HitReactMontage)
		{
			PlayAnimMontage(Data.HitReactMontage);
		}
		
		LaunchHitReaction();
		return;
	}
	
	if (HitReactionMap.Contains(DamageDirection))
	{
		const FDMC_HitReactionData& Data = HitReactionMap[DamageDirection];
		
		bool bIsInAir = GetCharacterMovement()->IsFalling() || GetCharacterMovement()->IsFlying();
		
		if (bIsInAir)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			
			UAnimMontage* MontageToPlay = Data.AirHitReactMontage ? Data.AirHitReactMontage.Get() : Data.HitReactMontage.Get();
			if (MontageToPlay)
			{
				PlayAnimMontage(MontageToPlay);
			}
		}
		else
		{
			if (Data.HitReactMontage)
			{
				PlayAnimMontage(Data.HitReactMontage);
			}
		}
		
		BufferComponent->StopBuffer();
		BufferComponent->StartBuffer(Data.PushbackAmount);
	}
}

void ADMC_EnemyCharacterBase::LaunchHitReaction()
{
	LaunchStartLocation = GetActorLocation();
	LaunchTargetLocation = LaunchStartLocation + FVector(0.f, 0.f, 400.f);
    
	if (LaunchCurve)
	{
		LaunchTimeline.PlayFromStart();
	}
}

void ADMC_EnemyCharacterBase::HandleLaunchTimelineProgress(float Value)
{
	FVector NewLocation = FMath::Lerp(LaunchStartLocation, LaunchTargetLocation, Value);
	SetActorLocation(NewLocation, true);
}