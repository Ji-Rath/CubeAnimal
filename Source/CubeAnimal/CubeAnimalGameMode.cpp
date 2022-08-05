// Copyright Epic Games, Inc. All Rights Reserved.

#include "CubeAnimalGameMode.h"
#include "CubeAnimalCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACubeAnimalGameMode::ACubeAnimalGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
