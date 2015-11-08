// Some copyright should be here...
// TCP implementation from: https://wiki.unrealengine.com/TCP_Socket_Listener,_Receive_Binary_Data_From_an_IP/Port_Into_UE4,_(Full_Code_Sample)

#include "TestPluginPrivatePCH.h"
#include <string>

#define LOCTEXT_NAMESPACE "FTestPluginModule"

DEFINE_LOG_CATEGORY(ModuleLog)
/*
//Rama's Start TCP Receiver
bool FTestPluginModule::StartTCPReceiver(
	const FString& YourChosenSocketName,
	const FString& TheIP,
	const int32 ThePort
	){
	//Rama's CreateTCPConnectionListener
	ListenerSocket = CreateTCPConnectionListener(YourChosenSocketName, TheIP, ThePort);

	//Not created?
	if (!ListenerSocket)
	{
		return false;
	}
	FTestPluginModule::TCPConnectionListener, 0.01, true);
	//Start the Listener! //thread this eventually
	//GetWorldTimerManager().SetTimer(this,
		//&FTestPluginModule::TCPConnectionListener, 0.01, true);

	return true;
}

//Rama's TCP Connection Listener
void FTestPluginModule::TCPConnectionListener()
{
	if (!ListenerSocket) return;

	//Remote address
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Pending;

	// handle incoming connections
	if (ListenerSocket->HasPendingConnection(Pending) && Pending)
	{
		//Already have a Connection? destroy previous
		if (ConnectionSocket)
		{
			ConnectionSocket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		}

		//New Connection receive!
		ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("RamaTCP Received Socket Connection"));

		if (ConnectionSocket != NULL)
		{
			//Global cache of current Remote Address
			RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);

			//UE_LOG "Accepted Connection! WOOOHOOOO!!!";

			FTestPluginModule::TCPSocketListener, 0.01, true);
			//can thread this too
			//GetWorldTimerManager().SetTimer(this,
				//&FTestPluginModule::TCPSocketListener, 0.01, true);
		}
	}
}

//Rama's TCP Socket Listener
void FTestPluginModule::TCPSocketListener()
{
	if (!ConnectionSocket) return;

	//Binary Array!
	TArray<uint8> ReceivedData;

	uint32 Size;
	while (ConnectionSocket->HasPendingData(Size))
	{
		ReceivedData.Init(FMath::Min(Size, 65507u));

		int32 Read = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

	}

	if (ReceivedData.Num() <= 0)
	{
		//No Data Received
		return;
	}

	UE_LOG(ModuleLog, Warning, TEXT("No of bytes: %d"), ReceivedData.Num());

	//						Rama's String From Binary Array
	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	UE_LOG(ModuleLog, Warning, TEXT("Send %d"), *ReceivedUE4String);
}

//Format IP String as Number Parts
bool FTestPluginModule::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4])
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
*/

//Rama's Create TCP Connection Listener
FSocket* FTestPluginModule::CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize)
{
	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(IP, IP4Nums))
	{
		return false;
	}

	//Create Socket
	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), Port);
	FSocket* ListenSocket = FTcpSocketBuilder(*SocketName)
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);

	//Done!
	return ListenSocket;
}

//Rama's String From Binary Array
//This function requires 
//		#include <string>
FString FTestPluginModule::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}

void FTestPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(ModuleLog, Warning, TEXT("Testing. testmodule"));
	//StartTCPReceiver("RamaSocketListener", "127.0.0.1", 8080, 1000);

	

	// server/listener
	FSocket* ListenerSocket = FTestPluginModule::CreateTCPConnectionListener("testListener", "127.0.0.1", 8080, 1000);


	// receiver
	bool Pending;
	if (ListenerSocket->HasPendingConnection(Pending) && Pending)
	{
		TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

		//New Connection receive!
		ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("RamaTCP Received Socket Connection"));

		// sender
		FSocket* Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
		FString address = TEXT("127.0.0.1");
		int32 port = 8080;
		FIPv4Address ip;
		FIPv4Address::Parse(address, ip);

		TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		addr->SetIp(ip.GetValue());
		addr->SetPort(port);

		bool connected = Socket->Connect(*addr);
		UE_LOG(ModuleLog, Warning, TEXT("Connected %d"), connected);

		TCHAR* msg = TEXT("abcdefgh");
		int32 size = FCString::Strlen(msg);
		int32 sent = 0;

		bool successful = Socket->Send((uint8*)TCHAR_TO_UTF8(msg), size, sent);
		UE_LOG(ModuleLog, Warning, TEXT("Send %d"), successful);

		// receiver - read data
		TArray<uint8> ReceivedData;

		uint32 Size;
		if (ConnectionSocket->HasPendingData(Size))
		{
			ReceivedData.Init(FMath::Min(Size, 65507u));

			int32 Read = 0;
			ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
			UE_LOG(ModuleLog, Warning, TEXT("Has data: %d"), ReceivedData.Num());

			const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
			UE_LOG(ModuleLog, Warning, TEXT("Data: %s"), *ReceivedUE4String);
		}
		
		if (GEngine)
			UE_LOG(ModuleLog, Warning, TEXT("ENGINE RUNNING"));
	}

	ListenerSocket->Close();
	
}

void FTestPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTestPluginModule, TestPlugin)