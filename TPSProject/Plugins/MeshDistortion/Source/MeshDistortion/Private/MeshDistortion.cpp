// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshDistortion.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FMeshDistortion"

void FMeshDistortion::StartupModule()
{
	FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MeshDistortion"))->GetBaseDir();
	if (!AllShaderSourceDirectoryMappings().Contains("/Plugins/MeshDistortion"))
	{
		AddShaderSourceDirectoryMapping("/Plugins/MeshDistortion", PluginDirectory);
	}
	UE_LOG(LogTemp, Warning, TEXT("MeshDistortion->StartupModule"));
}

void FMeshDistortion::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("MeshDistortion->ShutdownModule"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMeshDistortion, MeshDistortion)

