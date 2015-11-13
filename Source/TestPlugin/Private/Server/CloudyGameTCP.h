#pragma once

#include <string>

DECLARE_LOG_CATEGORY_EXTERN(ServerLog, Log, All)

class CloudyGameTCP
{
public:
	CloudyGameTCP(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize);
	~CloudyGameTCP();

	/** TCP implementation*/
	FSocket* ListenSocket;
	FTcpListener *TcpListener;
	//FSocket ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;

	bool CloudyGameTCP::CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize);
	bool CloudyGameTCP::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);
	FString CloudyGameTCP::StringFromBinaryArray(const TArray<uint8>& BinaryArray);
	FString CloudyGameTCP::ReceiveMsg(FSocket* ListenerSocket);
	bool CloudyGameTCP::HandleInput(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint);
	
};

