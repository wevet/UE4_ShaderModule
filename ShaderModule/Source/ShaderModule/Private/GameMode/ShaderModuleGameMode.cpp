// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameMode/ShaderModuleGameMode.h"
#include "Widget/ShaderModuleHUD.h"
#include "Character/ShaderModuleCharacter.h"
#include "UObject/ConstructorHelpers.h"


AShaderModuleGameMode::AShaderModuleGameMode() : Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = AShaderModuleHUD::StaticClass();
}
