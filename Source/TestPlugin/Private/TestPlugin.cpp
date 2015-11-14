// Some copyright should be here...
//TCP implementation from : https ://wiki.unrealengine.com/TCP_Socket_Listener,_Receive_Binary_Data_From_an_IP/Port_Into_UE4,_(Full_Code_Sample)

#include "TestPluginPrivatePCH.h"
#include "TestPlugin.h"
#include <string>

#define LOCTEXT_NAMESPACE "FTestPluginModule"

DEFINE_LOG_CATEGORY(ModuleLog)

#define SERVER_NAME "Listener"
#define IP "127.0.0.1"
#define PORT_NO 55555
#define BUFFER_SIZE 1024
#define JOIN_GAME "join"
#define QUIT_GAME "quit"
#define SUCCESS_MSG "Success"
#define FAILURE_MSG "Failure"

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

void FTestPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(ModuleLog, Warning, TEXT("TESTING!!"));

	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FTestPluginModule::Tick), 5.0f);

	// start the server (listener)
	FString data = "";
	//new CloudyGameTCP(SERVER_NAME, IP, PORT_NO, BUFFER_SIZE, this);

	uint8 IP4Nums[4];
	FormatIP4ToNumber(IP, IP4Nums);

	//Create Socket
	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), PORT_NO);
	FSocket* ListenSocket = FTcpSocketBuilder(SERVER_NAME).AsReusable().BoundToEndpoint(Endpoint).Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(BUFFER_SIZE, NewSize);


	FTcpListener* TcpListener = new FTcpListener(*ListenSocket, 0); // thread time
	TcpListener->OnConnectionAccepted().BindRaw(this, &FTestPluginModule::InputHandler);

	// initialise class variables
	InputStr = "";
	HasInputStrChanged = false;

}

bool FTestPluginModule::Tick(float DeltaTime)
{
	UE_LOG(ModuleLog, Warning, TEXT("Tick"));
	if (IsInRenderingThread())
		UE_LOG(ModuleLog, Warning, TEXT("is in rendering"));
	if (GIsRunning && IsInGameThread() && HasInputStrChanged) {
		UE_LOG(ModuleLog, Warning, TEXT("is in game"));

		FString Command, ControllerIdStr;
		int32 ControllerId;
		bool Success = false;

		InputStr.Split(" ", &Command, &ControllerIdStr, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

		ControllerId = FCString::Atoi(*ControllerIdStr);

		UE_LOG(ModuleLog, Warning, TEXT("Command: %s ControllerId: %d"), *Command, ControllerId);

		ExecuteCommand(Command, ControllerId);

		/*
		UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
		FString Error;
		GameInstance->CreateLocalPlayer(1, Error, true);
		*/
		InputStr = "";
		HasInputStrChanged = false;
	}
	return true;
}

//Rama's String From Binary Array
//This function requires #include <string>
FString FTestPluginModule::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}

bool FTestPluginModule::InputHandler(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint) {

	TArray<uint8> ReceivedData;
	uint32 Size;

	// wait for data to arrive
	while (!(ConnectionSocket->HasPendingData(Size)));

	// handle data - change current command
	UE_LOG(ModuleLog, Warning, TEXT("Has Data!!"));
	ReceivedData.Init(FMath::Min(Size, 65507u));

	int32 Read = 0;
	ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
	UE_LOG(ModuleLog, Warning, TEXT("Has data: %d"), ReceivedData.Num());

	FString ReceivedString = StringFromBinaryArray(ReceivedData);
	UE_LOG(ModuleLog, Warning, TEXT("Data: %s"), *ReceivedString);

	InputStr = ReceivedString;
	HasInputStrChanged = true;

	return true;
	
}

void FTestPluginModule::ExecuteCommand(FString Command, int32 ControllerId) {
	bool Success = false;
	if (GEngine)
	{
		UE_LOG(ModuleLog, Warning, TEXT("testing have engine"));

		UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();

		if (Command == JOIN_GAME)
		{
			FString Error;
			GameInstance->CreateLocalPlayer(ControllerId, Error, true);
			Success = true;
		}
		else if (Command == QUIT_GAME)
		{
			ULocalPlayer* const ExistingPlayer = GameInstance->FindLocalPlayerFromControllerId(ControllerId);
			if (ExistingPlayer != NULL)
			{
				GameInstance->RemoveLocalPlayer(ExistingPlayer);
				Success = true;
			}
		}

	}
}

void FTestPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTestPluginModule, TestPlugin)