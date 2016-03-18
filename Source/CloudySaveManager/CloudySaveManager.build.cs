using UnrealBuildTool;
using System.IO;
 
public class CloudySaveManager : ModuleRules
{
    public CloudySaveManager(TargetInfo Target)
    {
        PrivateIncludePaths.AddRange(
            new string[] {
                "CloudySaveManager/Private",
			}
        );
        PublicIncludePaths.AddRange(
            new string[] { 
                "CloudySaveManager/Public",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
				"Core",
				"CoreUObject",
                "Engine",
			}
        );
        PrivateDependencyModuleNames.AddRange(
            new string[] { 

            }
        );
    }
}