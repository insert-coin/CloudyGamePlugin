// Some copyright should be here...

using UnrealBuildTool;

public class TestPlugin : ModuleRules
{
    public TestPlugin(TargetInfo Target)
    {

        PublicIncludePaths.AddRange(
            new string[] {
				"TestPlugin/Public"
				
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
				"TestPlugin/Private",
				
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Sockets",
				"Networking"
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
			{
				"CoreUObject", "Engine", "Slate", "SlateCore"
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
