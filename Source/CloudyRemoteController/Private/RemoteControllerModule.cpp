#include "RemoteControllerPCH.h"

#include "RemoteControllerModule.h"

DEFINE_LOG_CATEGORY(ModuleLog)

void RemoteControllerModule::StartupModule()
{
	UE_LOG(ModuleLog, Warning, TEXT("CloudyGame: RemoteController Module Starting"));
	RestartServices();
}

void RemoteControllerModule::ShutdownModule()
{
	UE_LOG(ModuleLog, Warning, TEXT("CloudyGame: RemoteController Module Shutting Down"));
}

void RemoteControllerModule::RestartServices()
{
	InitializeRemoteServer();
}

void RemoteControllerModule::InitializeRemoteServer()
{
	FIPv4Endpoint ServerEndpoint;
	ServerEndpoint = CLOUDYGAME_REMOTE_CONTROLLER_SERVER_DEFAULT_ENDPOINT;
	RemoteServer = MakeShareable(new RemoteControllerServer(ServerEndpoint));
}

IMPLEMENT_MODULE(RemoteControllerModule, Module)