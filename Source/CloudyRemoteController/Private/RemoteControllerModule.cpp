#include "RemoteControllerPCH.h"

#include "RemoteControllerModule.h"

#define CLOUDYGAME_REMOTE_CONTROLLER_SERVER_DEFAULT_ENDPOINT FIPv4Endpoint(FIPv4Address(0, 0, 0, 0), 55555)

DEFINE_LOG_CATEGORY(RemoteControllerLog)

UGameInstance* gameinstance;
UWorld* world;

void RemoteControllerModule::StartupModule()
{
	UE_LOG(RemoteControllerLog, Warning, TEXT("CloudyGame: RemoteController Module Starting"));

    const FString& SocketName = "RemoteControllerSocket";
    const FString& IPAddress = "0.0.0.0";
    const int32 Port = 55555;

    world = NULL;

    InitializeRemoteServer(SocketName, IPAddress, Port);
}

void RemoteControllerModule::ShutdownModule()
{
	UE_LOG(RemoteControllerLog, Warning, TEXT("CloudyGame: RemoteController Module Shutting Down"));

    delete UDPInputReceiver;
    UDPInputReceiver = nullptr;

    if (ServerListenSocket)
    {
        ServerListenSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ServerListenSocket);
    }
}

void RemoteControllerModule::InitializeRemoteServer(const FString& SocketName, const FString& IPAddress, const int32 Port)
{
    FIPv4Address ParsedIP;
    FIPv4Address::Parse(IPAddress, ParsedIP);

    UE_LOG(RemoteControllerLog, Warning, TEXT("CloudyGame: RemoteController Server Starting"));

    // Create Socket
    FIPv4Endpoint Endpoint(ParsedIP, Port);
    FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
    ServerListenSocket = FUdpSocketBuilder(SocketName).AsNonBlocking().AsReusable().BoundToEndpoint(Endpoint);
    //ServerListenSocket = FUdpSocketBuilder(SocketName).AsNonBlocking().AsReusable().BoundToAddress(FIPv4Address::Any).BoundToPort(Port);
    UDPInputReceiver = new FUdpSocketReceiver(ServerListenSocket, ThreadWaitTime, TEXT("Udp Input Receiver"));
    UDPInputReceiver->OnDataReceived().BindRaw(this, &RemoteControllerModule::HandleInputReceived);
    UDPInputReceiver->Start(); // New in UE4 4.13
    
    UE_LOG(RemoteControllerLog, Warning, TEXT("CloudyGame: RemoteController Server Started Successfully"));
}

void RemoteControllerModule::ProcessKeyboardInput(const FArrayReaderPtr& Data)
{    
    FUdpRemoteControllerSegment::FKeyboardInputChunk Chunk;
	*Data << Chunk;

    // If world has not been loaded into variable yet
    if (world == NULL) {
        gameinstance = GEngine->GameViewport->GetGameInstance();
        world = gameinstance->GetWorld();
    }
	
	APlayerController* controller = UGameplayStatics::GetPlayerController(world, Chunk.ControllerID);

	if (Chunk.CharCode != 27) { // not ESC key (ESC crashes UE)
		EInputEvent ie;
		if (Chunk.InputEvent == 2){ // Pressed
			ie = EInputEvent::IE_Pressed;
		}
		else if (Chunk.InputEvent == 3){ // Released
			ie = EInputEvent::IE_Released;
		}
		FInputKeyManager manager = FInputKeyManager::Get();
		FKey key = manager.GetKeyFromCodes(Chunk.KeyCode, Chunk.CharCode);
		controller->InputKey(key, ie, 1, false);
	}
}


void RemoteControllerModule::ProcessMouseInput(const FArrayReaderPtr& Data)
{
    FUdpRemoteControllerSegment::FMouseInputChunk Chunk;
	*Data << Chunk;
	
    // If world has not been loaded into variable yet
    if (world == NULL) {
        gameinstance = GEngine->GameViewport->GetGameInstance();
        world = gameinstance->GetWorld();
    }

	APlayerController* controller = UGameplayStatics::GetPlayerController(world, Chunk.ControllerID);

    // InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
    if (Chunk.XAxis != NULL)
    {
        controller->InputAxis(EKeys::MouseX, Chunk.XAxis, world->GetDeltaSeconds(), 1, false);
    }
    if (Chunk.YAxis != NULL)
    {
        controller->InputAxis(EKeys::MouseY, -Chunk.YAxis, world->GetDeltaSeconds(), 1, false);
    }
}

void RemoteControllerModule::HandleInputReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Sender)
{
    FUdpRemoteControllerSegment::FHeaderChunk Chunk;
    *Data << Chunk;

    switch (Chunk.SegmentType)
    {
    case EUdpRemoteControllerSegment::KeyboardInput:
    	ProcessKeyboardInput(Data);
        //(new FAutoDeleteAsyncTask<CloudyProcessKeyboardInput>(Data))->StartBackgroundTask();
    	break;
    case EUdpRemoteControllerSegment::MouseInput:
    	ProcessMouseInput(Data);
        //(new FAutoDeleteAsyncTask<CloudyProcessMouseInput>(Data))->StartBackgroundTask();
       	break;
    }
}

IMPLEMENT_MODULE(RemoteControllerModule, Module)