#include "TestPlugin2PrivatePCH.h"

DEFINE_LOG_CATEGORY(ServerLog)

CloudyGameTCPServer::CloudyGameTCPServer(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize)
{
	ListenerSocket = CloudyGameTCPServer::CreateTCPConnectionListener(SocketName, IP, Port, ReceiveBufferSize);
	//FString msg = CloudyGameTCPServer::ReceiveMsg(ListenerSocket);

}


CloudyGameTCPServer::~CloudyGameTCPServer()
{
}

//Rama's Create TCP Connection Listener
FSocket* CloudyGameTCPServer::CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize)
{
	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(IP, IP4Nums))
		return false;

	//Create Socket
	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), Port);
	FSocket* ListenSocket = FTcpSocketBuilder(*SocketName).AsReusable().BoundToEndpoint(Endpoint).Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);

	//Done!
	return ListenSocket;
}

//Format IP String as Number Parts
bool CloudyGameTCPServer::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4])
{
	//IP Formatting
	TheIP.Replace(TEXT(" "), TEXT(""));

	//String Parts
	TArray<FString> Parts;
	TheIP.ParseIntoArray(Parts, TEXT("."), true);
	if (Parts.Num() != 4)
		return false;

	//String to Number Parts
	for (int32 i = 0; i < 4; ++i)
		Out[i] = FCString::Atoi(*Parts[i]);

	return true;
}

//Rama's String From Binary Array
//This function requires #include <string>
FString CloudyGameTCPServer::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}

FString CloudyGameTCPServer::ReceiveMsg(FSocket* ListenerSocket)
{
	// receiver
	bool Pending;
	while (!(ListenerSocket->HasPendingConnection(Pending) && Pending)) {
		Sleep(1000);
	}
	if (ListenerSocket->HasPendingConnection(Pending) && Pending)
	{
		TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

		//New Connection receive!
		ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("RamaTCP Received Socket Connection"));

		// receiver - read data
		TArray<uint8> ReceivedData;

		uint32 Size;
		if (ConnectionSocket->HasPendingData(Size))
		{
			ReceivedData.Init(FMath::Min(Size, 65507u));

			int32 Read = 0;
			ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
			UE_LOG(ServerLog, Warning, TEXT("Has data: %d"), ReceivedData.Num());

			const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
			UE_LOG(ServerLog, Warning, TEXT("Data: %s"), *ReceivedUE4String);
			return ReceivedUE4String;
		}
	}
	return "NO_MSG";
}
