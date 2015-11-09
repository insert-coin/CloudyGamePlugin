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

	UE_LOG(ServerLog, Warning, TEXT("CloudyGame: RemoteController Handling Data"));
	char* charData = (char*)Data->GetData(); 
	UE_LOG(ServerLog, Warning, TEXT("Data: %s"), UTF8_TO_TCHAR(charData));
	UGameInstance* gameinstance = GEngine->GameViewport->GetGameInstance();
	UE_LOG(ServerLog, Warning, TEXT("Game Instance found"));
	UWorld* world = gameinstance->GetWorld();
	//APlayerController* controller = UGameplayStatics::GetPlayerController(world, Chunk.ControllerID);
	APlayerController* controller = gameinstance->GetFirstLocalPlayerController();
	UE_LOG(ServerLog, Warning, TEXT("Player controller [%d] found"), Chunk.ControllerID);
	
	UE_LOG(ServerLog, Warning, TEXT("KEY %d"), Chunk.KeyCode);
	EInputEvent ie;
	if (Chunk.InputEvent == 2){ // Pressed
		ie = EInputEvent::IE_Pressed;
		UE_LOG(ServerLog, Warning, TEXT("Input Pressed"));
	}
	else if (Chunk.InputEvent == 3){ // Released
		ie = EInputEvent::IE_Released;
		UE_LOG(ServerLog, Warning, TEXT("Input Released"));
	}

	
	//UE_LOG(ServerLog, Warning, TEXT("Sequence %d"), Chunk.Sequence);
	FInputKeyManager manager = FInputKeyManager::Get();
	FKey key = manager.GetKeyFromCodes(Chunk.KeyCode, Chunk.KeyCode);
	//UE_LOG(ServerLog, Warning, TEXT("Input Key Executed %d"), Chunk.KeyCode);
	//UE_LOG(ServerLog, Warning, TEXT("Unicode %d"), Chunk.Unicode);
	//FKey key = FKey::FKey(UTF8_TO_TCHAR(Chunk.Unicode));
	controller->InputKey(key, ie, 1, false);
	//controller->InputKey(FKey(UTF8_TO_TCHAR("w")), ie, 1, false);
	UE_LOG(ServerLog, Warning, TEXT("Done"));
}