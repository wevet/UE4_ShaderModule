// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Sketch : ModuleRules
{
	public Sketch(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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
			"CoreUObject",
			"Engine",
			"RHI",
			"RenderCore",
			"AssetTools",
			"Slate",
			"SlateCore",
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
