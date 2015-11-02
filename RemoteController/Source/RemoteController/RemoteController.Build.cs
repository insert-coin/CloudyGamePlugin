using UnrealBuildTool;
using System.IO;

public class RemoteController : ModuleRules
{
	public RemoteController(TargetInfo Target)
	{
		PrivateIncludePaths.AddRange(new string[] {
			"RemoteController/Private",
			"RemoteController/Private/Server",
		});
		PublicIncludePaths.AddRange(new string[] { "RemoteController/Public" });

		PublicDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "Sockets", "Networking", "InputCore" });
	}
}