using UnrealBuildTool;

public class Grid : ModuleRules
{
    public Grid(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "EnTT", 
                "ENTT_Common", 
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
            }
        );
    }
}