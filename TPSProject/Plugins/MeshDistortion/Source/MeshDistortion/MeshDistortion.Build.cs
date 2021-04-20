// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MeshDistortion : ModuleRules
{
	public MeshDistortion(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] 
		{
			//
		});


		PrivateIncludePaths.AddRange(new string[] 
		{
			//
		});


		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"RHI",
			"RenderCore",
		});


		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Projects",
		});


		DynamicallyLoadedModuleNames.AddRange(new string[]
		{

		});
	}
}
