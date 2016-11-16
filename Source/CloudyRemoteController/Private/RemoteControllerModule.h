#pragma once

#include "Engine.h"

DECLARE_LOG_CATEGORY_EXTERN(RemoteControllerLog, Log, All)


class RemoteControllerModule : public IModuleInterface
{
public:
	void StartupModule();
	void ShutdownModule();

protected:
	void InitializeRemoteServer(const FString& SocketName, const FString& IPAddress, const int32 Port);

private:
	FSocket* ServerListenSocket;
    FUdpSocketReceiver* UDPInputReceiver;

    void HandleInputReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Sender);
    void ProcessMouseInput(const FArrayReaderPtr& Data);
    void ProcessKeyboardInput(const FArrayReaderPtr& Data);
};