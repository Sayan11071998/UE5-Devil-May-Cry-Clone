#include "Enemies/DMC_EnemyCharacterBase.h"
#include "DamageTypes/DMC_DamageType.h"
#include "Engine/DamageEvents.h"

ADMC_EnemyCharacterBase::ADMC_EnemyCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

float ADMC_EnemyCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
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
		MontageToPlay = HitReactionLeft;
		break;
	case EDMC_DamageType::EDT_Right:
		MontageToPlay = HitReactionRight;
		break;
	case EDMC_DamageType::EDT_Middle:
		MontageToPlay = HitReactionMiddle;
		break;
	case EDMC_DamageType::EDT_KnockBack:
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