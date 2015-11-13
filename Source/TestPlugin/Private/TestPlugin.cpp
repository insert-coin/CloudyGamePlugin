// Some copyright should be here...
//TCP implementation from : https ://wiki.unrealengine.com/TCP_Socket_Listener,_Receive_Binary_Data_From_an_IP/Port_Into_UE4,_(Full_Code_Sample)

#include "TestPluginPrivatePCH.h"
#include "TestPlugin.h"

#define LOCTEXT_NAMESPACE "FTestPluginModule"

DEFINE_LOG_CATEGORY(ModuleLog)

#define SERVER_NAME "Listener"
#define IP "127.0.0.1"
#define PORT_NO 55555
#define BUFFER_SIZE 1024
#define JOIN_GAME "join"
#define QUIT_GAME "quit"

void FTestPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(ModuleLog, Warning, TEXT("TESTING!!"));

	// start the server (listener)
	//std::thread ListeningThread(CloudyGameTCP, "testListener", "127.0.0.1", 8080, 1000);
	new CloudyGameTCP(SERVER_NAME, IP, PORT_NO, BUFFER_SIZE);

	
	// sender
	/*
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
	*/

	// server listen
	//FString test = FTestPluginModule::ReceiveMsg(ListenerSocket);
	//UE_LOG(ModuleLog, Warning, TEXT("Data received by server: %s"), *test);
	
	//ListenerSocket->Close();

}

void FTestPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTestPluginModule, TestPlugin)