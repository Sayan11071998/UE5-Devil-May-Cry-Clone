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
	
	UDMC_DamageType* DamageType = Cast<UDMC_DamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	if (DamageType)
	{
		EDMC_DamageType HitDirection = DamageType->DamageType;
		FString EnumValueString = UEnum::GetValueAsString(HitDirection);
		UE_LOG(LogTemp, Warning, TEXT("Enemy Hit! Damage Type: %s"), *EnumValueString);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("Hit Direction: %s"), *EnumValueString));
		}
	}
	return ActualDamage;
}