// Copyright Epic Games, Inc. All Rights Reserved.

#include "Sketch.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FSketchModule"

void FSketchModule::StartupModule()
{
	FString PluginDirectory =FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("Sketch"))->GetBaseDir(), TEXT("Shader"));
	AddShaderSourceDirectoryMapping("/Sketch", PluginDirectory);
	UE_LOG(LogTemp, Warning, TEXT("SketchModule->StartupModule"));
}

void FSketchModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("SketchModule->ShutdownModule"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSketchModule, Sketch)

