using UnrealBuildTool;

public class SpatialPlus : ModuleRules
{
	public SpatialPlus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"SpatialPlus/Public"
			});

		PrivateIncludePaths.AddRange(
			new string[] {
				"SpatialPlus/Private",
			});

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