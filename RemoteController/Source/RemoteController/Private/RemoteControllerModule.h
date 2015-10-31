#pragma once

DECLARE_LOG_CATEGORY_EXTERN(ModuleLog, Log, All)


class RemoteControllerModule : public IModuleInterface
{
public:
	void StartupModule();
	void ShutdownModule();

protected:
	void RestartServices();
	void InitializeRemoteServer();

private:
	FSocket* ServerSocket;
	TSharedPtr<RemoteControllerServer> RemoteServer;
};