#include "RemoteControllerPCH.h"

DECLARE_LOG_CATEGORY_EXTERN(ModuleLog, Log, All)
DEFINE_LOG_CATEGORY(ModuleLog)

class RemoteControllerModule : public IModuleInterface
{
public:
	void StartupModule(){
		UE_LOG(ModuleLog, Warning, TEXT("CloudyGame: RemoteController Starting"));
		RestartServices();
    }

	void ShutdownModule()
	{
		UE_LOG(ModuleLog, Warning, TEXT("CloudyGame: RemoteController Shutting Down"));
	}

protected:
	void RestartServices(){
		// TODO: Run the Server
	}
};

IMPLEMENT_MODULE(RemoteControllerModule, Module)