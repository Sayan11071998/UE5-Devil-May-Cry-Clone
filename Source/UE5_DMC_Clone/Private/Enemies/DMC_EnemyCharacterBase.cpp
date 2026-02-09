#include "Enemies/DMC_EnemyCharacterBase.h"
#include "DamageTypes/DMC_DamageType.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"

ADMC_EnemyCharacterBase::ADMC_EnemyCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADMC_EnemyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateBufferLogic(DeltaTime);
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

void ADMC_EnemyCharacterBase::UpdateBufferLogic(float DeltaTime)
{
	if (!bIsBuffering || !BufferCurve) return;
	
	BufferTimeElapsed += DeltaTime;
		
	if (BufferTimeElapsed <= BufferDuration)
	{
		float Alpha = BufferCurve->GetFloatValue(BufferTimeElapsed);
		FVector CurrentLoc = GetActorLocation();
		FVector Forward = GetActorForwardVector();
		
		// The 60.f ensures movement feels the same regardless of frame rate
		FVector TargetLoc = CurrentLoc + (Forward * CurrentBufferAmount);
		FVector NewLoc = FMath::Lerp(CurrentLoc, TargetLoc, Alpha * DeltaTime * 60.f);
			
		SetActorLocation(NewLoc, true);
	}
	else
	{
		StopBuffer();
	}
}

void ADMC_EnemyCharacterBase::PlayHitReaction(EDMC_DamageType DamageDirection)
{
	if (HitReactionMap.Contains(DamageDirection))
	{
		const FDMC_HitReactionData& Data = HitReactionMap[DamageDirection];
		
		StartBuffer(Data.PushbackAmount);
		if (Data.HitReactMontage)
		{
			PlayAnimMontage(Data.HitReactMontage);
		}
	}
}

void ADMC_EnemyCharacterBase::StartBuffer(float Amount)
{
	CurrentBufferAmount = Amount;
	BufferTimeElapsed = 0.f;
	bIsBuffering = true;
}

void ADMC_EnemyCharacterBase::StopBuffer()
{
	bIsBuffering = false;
}