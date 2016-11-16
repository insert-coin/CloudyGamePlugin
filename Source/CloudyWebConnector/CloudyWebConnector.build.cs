using UnrealBuildTool;
using System.IO;
 
public class CloudyWebConnector : ModuleRules
{
    public CloudyWebConnector(TargetInfo Target)
    {
        PrivateIncludePaths.AddRange(
            new string[] {
                "CloudyWebConnector/Private",
            }
        );
        PublicIncludePaths.AddRange(
            new string[] { 
                "CloudyWebConnector/Public",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "Json",
                "Http",
                "Sockets",
                "Networking"
            }
        );
        PrivateDependencyModuleNames.AddRange(new string[] { 
            "JsonUtilities",
            "Sockets",
            "Networking"
            }
        );
    }
}