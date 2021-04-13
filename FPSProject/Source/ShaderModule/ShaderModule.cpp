// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShaderModule.h"

 
void FShaderModule::StartupModule()
{
#if (ENGINE_MINOR_VERSION >= 21)
	FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shader"));
	if (!AllShaderSourceDirectoryMappings().Contains("/Project"))
	{
		AddShaderSourceDirectoryMapping("/Project", ShaderDirectory);
	}
#endif

	UE_LOG(LogTemp, Warning, TEXT("FShaderModule::StartupModule"));
}

void FShaderModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("FShaderModule::ShutdownModule"));
}


IMPLEMENT_GAME_MODULE(FShaderModule, ShaderModule);

