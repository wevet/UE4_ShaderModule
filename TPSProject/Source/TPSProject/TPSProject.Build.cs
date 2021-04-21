// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TPSProject : ModuleRules
{
	public TPSProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/TPSProject.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		bUseUnity = true;

		PrivateIncludePaths.Add("TPSProject/Private");

		PublicDependencyModuleNames.AddRange(new string[]
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay",

			// Custom Plugins
			"Sketch",
			"MeshDistortion",
			"VolumetricClouds",
		});

	}
}
