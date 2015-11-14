#include "RemoteControllerPCH.h"

#include "RemoteControllerServer.h"

DEFINE_LOG_CATEGORY(ServerLog)

RemoteControllerServer::RemoteControllerServer(const FIPv4Endpoint& InServerEndpoint)
{
	StartServer(InServerEndpoint);
}

RemoteControllerServer::~RemoteControllerServer()
{
}

bool RemoteControllerServer::StartServer(const FIPv4Endpoint& endpoint)
{
	UE_LOG(ServerLog, Warning, TEXT("CloudyGame: RemoteController Server Started"));
	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	ServerSocket = FUdpSocketBuilder(TEXT("RemoteControllerServerSocket")).BoundToEndpoint(endpoint);
	InputReceiver = new FUdpSocketReceiver(ServerSocket, ThreadWaitTime, TEXT("Udp Input Receiver"));
	InputReceiver->OnDataReceived().BindRaw(this, &RemoteControllerServer::HandleInputReceived);
	UE_LOG(ServerLog, Warning, TEXT("CloudyGame: RemoteController Server Started Successfully"));
	return true;
}

void RemoteControllerServer::HandleInputReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Sender)
{
	FUdpRemoteControllerSegment::FKeyBoardChunk Chunk;
	*Data << Chunk;
	char* charData = (char*)Data->GetData(); 
	UGameInstance* gameinstance = GEngine->GameViewport->GetGameInstance();
	UWorld* world = gameinstance->GetWorld();
	APlayerController* controller = UGameplayStatics::GetPlayerController(world, Chunk.ControllerID);
	EInputEvent ie;
	if (Chunk.InputEvent == 2){ // Pressed
		ie = EInputEvent::IE_Pressed;
	}
	else if (Chunk.InputEvent == 3){ // Released
		ie = EInputEvent::IE_Released;
	}
	FInputKeyManager manager = FInputKeyManager::Get();
	FKey key = manager.GetKeyFromCodes(Chunk.KeyCode, Chunk.KeyCode);
	controller->InputKey(key, ie, 1, false);
}