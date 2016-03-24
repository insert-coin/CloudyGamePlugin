// Some copyright should be here...

using UnrealBuildTool;

public class CloudyStream : ModuleRules
{
    public CloudyStream(TargetInfo Target)
    {

        PublicIncludePaths.AddRange(
            new string[] {
				"CloudyStream/Public"
				
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
				"CloudyStream/Private",
				
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
			{
				"Core",
                "Engine",
                "Json",
                "Http"
				
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
