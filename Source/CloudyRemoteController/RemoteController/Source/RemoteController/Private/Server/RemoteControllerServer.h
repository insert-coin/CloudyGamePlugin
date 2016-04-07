#pragma once
#include "Engine.h"
DECLARE_LOG_CATEGORY_EXTERN(ServerLog, Log, All)

class RemoteControllerServer
{
public:
	RemoteControllerServer(const FIPv4Endpoint& InServerEndpoint);
	~RemoteControllerServer();

	bool StartServer(const FIPv4Endpoint& ServerEndpoint);
	void StopServer();
private:
	void ProcessKeyboardInput(const FArrayReaderPtr& Data);
	void ProcessMouseInput(const FArrayReaderPtr& Data);
	void HandleInputReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Sender);
	FSocket* ServerSocket;
	FUdpSocketReceiver* InputReceiver;
};