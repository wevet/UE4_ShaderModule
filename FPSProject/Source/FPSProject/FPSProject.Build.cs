// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPSProject : ModuleRules
{
	public FPSProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/FPSProject.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		bUseUnity = true;

		PrivateIncludePaths.Add("FPSProject/Private");

		PublicDependencyModuleNames.AddRange(new string[]
			{ 
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore", 
				"HeadMountedDisplay",
				"ShaderModule",
			}
		);
	}
}

