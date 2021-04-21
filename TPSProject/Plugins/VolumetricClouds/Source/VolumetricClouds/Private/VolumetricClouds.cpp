// Copyright Epic Games, Inc. All Rights Reserved.

#include "VolumetricClouds.h"

#define LOCTEXT_NAMESPACE "FVolumetricCloudsModule"

void FVolumetricCloudsModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("VolumetricCloudsModule->StartupModule"));
}

void FVolumetricCloudsModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("VolumetricCloudsModule->ShutdownModule"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVolumetricCloudsModule, VolumetricClouds)
