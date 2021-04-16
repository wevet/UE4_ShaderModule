// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameMode/TPSProjectGameMode.h"
#include "Character/TPSProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATPSProjectGameMode::ATPSProjectGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));

	if (PlayerPawnBPClass.Class)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
