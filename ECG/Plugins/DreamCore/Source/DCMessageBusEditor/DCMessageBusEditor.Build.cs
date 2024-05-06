using UnrealBuildTool;

public class DCMessageBusEditor : ModuleRules
{
    public DCMessageBusEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        //OverridePackageType = PackageOverrideType.GameUncookedOnly;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
        
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "DCMessageBus",
            "BlueprintGraph",
            "KismetCompiler",
            "UnrealEd"
        });
    }
}