// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShaderModule : ModuleRules
{
	public ShaderModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/ShaderModule.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		bUseUnity = true;

		PrivateIncludePaths.Add("ShaderModule/Private");

		PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore", 
				"HeadMountedDisplay" 
			}
		);
	}
}

