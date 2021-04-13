// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShaderModule : ModuleRules
{
	public ShaderModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{ 
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore",
				"RenderCore",
				"RHI",
			}
		);
	}
}

