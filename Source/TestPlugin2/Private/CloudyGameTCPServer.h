#pragma once

#include "TestPlugin2PrivatePCH.h"
#include <string>

DECLARE_LOG_CATEGORY_EXTERN(ServerLog, Log, All)

class CloudyGameTCPServer
{
public:
	CloudyGameTCPServer(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize);
	~CloudyGameTCPServer();

	/** TCP implementation*/

	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;

	FSocket* CloudyGameTCPServer::CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize);
	bool CloudyGameTCPServer::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);
	FString CloudyGameTCPServer::StringFromBinaryArray(const TArray<uint8>& BinaryArray);
	FString CloudyGameTCPServer::ReceiveMsg(FSocket* ListenerSocket);

};

