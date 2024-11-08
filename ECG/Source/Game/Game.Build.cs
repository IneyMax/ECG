﻿using UnrealBuildTool;

public class Game : ModuleRules
{
    public Game(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "EnTT", 
                "Cards",
                "StructUtils", 
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
                "EntityStorage", 
                "ENTT_Common", 
                "Cards", 
                "Grid"
            }
        );
    }
}