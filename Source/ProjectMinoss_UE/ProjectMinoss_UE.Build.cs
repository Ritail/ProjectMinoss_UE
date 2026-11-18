// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ProjectMinoss_UE : ModuleRules
{
	public ProjectMinoss_UE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		PublicDependencyModuleNames.AddRange(new string[] {

			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			"GameplayTags", "Json", "JsonUtilities" // ajout

		});
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
