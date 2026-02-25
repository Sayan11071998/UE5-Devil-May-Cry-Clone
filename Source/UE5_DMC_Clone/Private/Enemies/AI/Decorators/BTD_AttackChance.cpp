#include "Enemies/AI/Decorators/BTD_AttackChance.h"

UBTD_AttackChance::UBTD_AttackChance()
{
	NodeName = "Attack Chance Decorator";
	Chance = 50;
}

bool UBTD_AttackChance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const int32 RandomValue = FMath::RandRange(0, 100);
	return RandomValue <= Chance;
}