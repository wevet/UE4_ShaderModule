
using UnrealBuildTool;

public class VolumetricCloudsPainter : ModuleRules
{
	public VolumetricCloudsPainter(ReadOnlyTargetRules Target) : base(Target)
	{
		//PrivatePCHHeaderFile = "Public/VolumetricCloudsPainter.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		//PrivateIncludePaths.Add("VolumetricCloudsPainter/Private");

		PublicIncludePaths.AddRange(
			new string[] 
			{
			}
		);


		PrivateIncludePaths.AddRange(
			new string[] 
			{
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"VolumetricClouds",	
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"EditorStyle"
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
