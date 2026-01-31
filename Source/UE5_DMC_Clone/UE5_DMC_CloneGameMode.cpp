// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_DMC_CloneGameMode.h"
#include "UE5_DMC_CloneCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE5_DMC_CloneGameMode::AUE5_DMC_CloneGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
