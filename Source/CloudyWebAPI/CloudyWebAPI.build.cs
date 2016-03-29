using UnrealBuildTool;
using System.IO;
 
public class CloudyWebAPI : ModuleRules
{
    public CloudyWebAPI(TargetInfo Target)
    {
        PrivateIncludePaths.AddRange(
            new string[] {
                "CloudyWebAPI/Private",
            }
        );
        PublicIncludePaths.AddRange(
            new string[] { 
                "CloudyWebAPI/Public",
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

        // Curl
        string LibCurlPath = UEBuildConfiguration.UEThirdPartySourceDirectory + "libcurl/";
        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicIncludePaths.Add(LibCurlPath + "include/Linux/" + Target.Architecture);
            PublicLibraryPaths.Add(LibCurlPath + "lib/Linux/" + Target.Architecture);
            PublicAdditionalLibraries.Add("curl");
            PublicAdditionalLibraries.Add("crypto");
            PublicAdditionalLibraries.Add("ssl");
            PublicAdditionalLibraries.Add("dl");
        }
        else if (Target.Platform == UnrealTargetPlatform.Win32 ||
                 Target.Platform == UnrealTargetPlatform.Win64 || (Target.Platform == UnrealTargetPlatform.HTML5 && Target.Architecture == "-win32"))
        {
            PublicIncludePaths.Add(LibCurlPath + "include/Windows");

            string LibCurlLibPath = LibCurlPath + "lib/";
            LibCurlLibPath += (Target.Platform == UnrealTargetPlatform.Win64) ? "Win64/" : "Win32/";
            LibCurlLibPath += "VS" + WindowsPlatform.GetVisualStudioCompilerVersionName();
            PublicLibraryPaths.Add(LibCurlLibPath);

            PublicAdditionalLibraries.Add("libcurl_a.lib");
            Definitions.Add("CURL_STATICLIB=1");
        }
    }
}