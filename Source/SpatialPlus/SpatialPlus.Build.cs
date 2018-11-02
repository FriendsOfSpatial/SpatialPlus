using UnrealBuildTool;

public class SpatialPlus : ModuleRules
{
	public SpatialPlus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

	    if (Target.Version.MinorVersion <= 19)
	    {
            PublicIncludePaths.AddRange(
			    new string[] {
				    "Public"
			    });

		    PrivateIncludePaths.AddRange(
			    new string[] {
				    "Private",
			    });
        }

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",

                "SpatialOS"
			});

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			});
	}
}
