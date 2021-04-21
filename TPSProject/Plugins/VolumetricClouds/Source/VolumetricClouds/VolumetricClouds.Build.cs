// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VolumetricClouds : ModuleRules
{
	public VolumetricClouds(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[]
		{
			//
		});


		PrivateIncludePaths.AddRange(new string[]
		{
			// ... add other private include paths required here ...
		});


		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
		});


		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			//"UnrealEd",
			// ... add private dependencies that you statically link with here ...	
		});


		DynamicallyLoadedModuleNames.AddRange(new string[]
		{
			//
		});
	}
}
