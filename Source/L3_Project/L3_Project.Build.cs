// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class L3_Project : ModuleRules
{
	public L3_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", 
	        "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemEOS", "OnlineSubsystemUtils", "HTTP", "Json", "JsonUtilities"});
    }
}
