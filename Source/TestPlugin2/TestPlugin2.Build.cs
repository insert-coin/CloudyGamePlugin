// Some copyright should be here...

using UnrealBuildTool;

public class TestPlugin2 : ModuleRules
{
	public TestPlugin2(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"TestPlugin2/Public"
				
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"TestPlugin2/Private",
				
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject", "Engine", "Slate", "SlateCore",
                "Engine",
                "Networking",
                "Core",
				"Engine",
				"Sockets"
				
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
