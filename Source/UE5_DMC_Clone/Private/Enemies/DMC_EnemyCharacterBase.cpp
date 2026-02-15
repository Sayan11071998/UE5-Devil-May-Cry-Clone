#include "Enemies/DMC_EnemyCharacterBase.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "DamageTypes/DMC_DamageType.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DMC_CombatBufferComponent.h"

ADMC_EnemyCharacterBase::ADMC_EnemyCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Health = MaxHealth;
	
	BufferComponent = CreateDefaultSubobject<UDMC_CombatBufferComponent>(TEXT("CombatBuffer"));
}

void ADMC_EnemyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADMC_EnemyCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (bDead) return 0.f;
	
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		SpawnHitFX(DamageCauser, PointDamageEvent->HitInfo.ImpactPoint);
	}

	if (DamageCauser)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DamageCauser->GetActorLocation());
		SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));
	}

	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);

	if (Health <= 0.f)
	{
		Death();
	}
	else
	{
		if (DamageEvent.DamageTypeClass)
		{
			UDMC_DamageType* DamageTypeObject = Cast<UDMC_DamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageTypeObject)
			{
				PlayHitReaction(DamageTypeObject->DamageType);
			}
		}
	}
	
	return DamageAmount;
}

void ADMC_EnemyCharacterBase::Finished(AActor* PlayerAttacker)
{
	bDead = true;
	
	if (PlayerAttacker)
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerAttacker->GetActorLocation());
		SetActorRotation(FRotator(0.f, LookAtRot.Yaw, 0.f));
	}

	if (FinishedMontage)
	{
		PlayAnimMontage(FinishedMontage);
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADMC_EnemyCharacterBase::SpawnHitFX(AActor* DamageCauser, const FVector& ImpactPoint)
{
	if (!HitVFX || !DamageCauser) return;
	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(DamageCauser->GetActorLocation(), GetActorLocation());
	LookAtRot.Yaw += 90.f;
    
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitVFX, ImpactPoint, LookAtRot);
}

void ADMC_EnemyCharacterBase::PlayHitReaction(EDMC_DamageType DamageDirection)
{
	if (HitReactionMap.Contains(DamageDirection))
	{
		const FDMC_HitReactionData& Data = HitReactionMap[DamageDirection];
		
		BufferComponent->StopBuffer();
		BufferComponent->StartBuffer(Data.PushbackAmount);
		if (Data.HitReactMontage)
		{
			PlayAnimMontage(Data.HitReactMontage);
		}
	}
}

void ADMC_EnemyCharacterBase::Death()
{
	if (bDead) return;
	bDead = true;
	
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}