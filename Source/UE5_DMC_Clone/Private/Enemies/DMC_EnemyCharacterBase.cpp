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
	
	if (bIsBuffering && BufferCurve)
	{
		BufferTimeElapsed += DeltaTime;
		
		if (BufferTimeElapsed <= BufferDuration)
		{
			float Alpha = BufferCurve->GetFloatValue(BufferTimeElapsed);
			
			FVector CurrentLoc = GetActorLocation();
			FVector Forward = GetActorForwardVector();
			FVector TargetLoc = CurrentLoc + (Forward * CurrentBufferAmount);
			FVector NewLoc = FMath::Lerp(CurrentLoc, TargetLoc, Alpha * DeltaTime * 60.f);
			
			SetActorLocation(NewLoc, true);
		}
		else
		{
			StopBuffer();
		}
	}
}

float ADMC_EnemyCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (DamageCauser)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DamageCauser->GetActorLocation());
		LookAtRotation.Pitch = 0.f;
		LookAtRotation.Roll = 0.f;
		SetActorRotation(LookAtRotation);
	}
	
	UDMC_DamageType* DamageTypeObject = Cast<UDMC_DamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	if (DamageTypeObject)
	{
		EDMC_DamageType HitDirection = DamageTypeObject->DamageType;
		PlayHitReaction(HitDirection);
	}
	return ActualDamage;
}

void ADMC_EnemyCharacterBase::PlayHitReaction(EDMC_DamageType DamageDirection)
{
	UAnimMontage* MontageToPlay = nullptr;

	switch (DamageDirection)
	{
	case EDMC_DamageType::EDT_Left:
		StartBuffer(-6.0f);
		MontageToPlay = HitReactionLeft;
		break;
	case EDMC_DamageType::EDT_Right:
		StartBuffer(-6.0f);
		MontageToPlay = HitReactionRight;
		break;
	case EDMC_DamageType::EDT_Middle:
		StartBuffer(-6.0f);
		MontageToPlay = HitReactionMiddle;
		break;
	case EDMC_DamageType::EDT_KnockBack:
		StartBuffer(-10.0f);
		MontageToPlay = HitReactionKnockBack;
		break;
	default:
		break;
	}
	
	if (MontageToPlay)
	{
		PlayAnimMontage(MontageToPlay);
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