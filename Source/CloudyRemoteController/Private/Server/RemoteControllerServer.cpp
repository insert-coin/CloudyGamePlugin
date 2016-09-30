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
	ServerSocket = FUdpSocketBuilder(TEXT("RemoteControllerServerSocket")).AsNonBlocking().BoundToEndpoint(endpoint);
	InputReceiver = new FUdpSocketReceiver(ServerSocket, ThreadWaitTime, TEXT("Udp Input Receiver"));
	InputReceiver->OnDataReceived().BindRaw(this, &RemoteControllerServer::HandleInputReceived);
	UE_LOG(ServerLog, Warning, TEXT("CloudyGame: RemoteController Server Started Successfully"));
	return true;
}

void RemoteControllerServer::ProcessKeyboardInput(const FArrayReaderPtr& Data)
{
	FUdpRemoteControllerSegment::FKeyboardInputChunk Chunk;
	*Data << Chunk;
	UGameInstance* gameinstance = GEngine->GameViewport->GetGameInstance();
	UWorld* world = gameinstance->GetWorld();
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


void RemoteControllerServer::ProcessMouseInput(const FArrayReaderPtr& Data)
{
	FUdpRemoteControllerSegment::FMouseInputChunk Chunk;
	*Data << Chunk;
	UGameInstance* gameinstance = GEngine->GameViewport->GetGameInstance();
	UWorld* world = gameinstance->GetWorld();
	APlayerController* controller = UGameplayStatics::GetPlayerController(world, Chunk.ControllerID);

	// InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
	FKey mouseX = EKeys::MouseX;
	FKey mouseY = EKeys::MouseY;
    controller->InputAxis(mouseX, Chunk.XAxis, world->GetDeltaSeconds(), 1, false);
    controller->InputAxis(mouseY, -Chunk.YAxis, world->GetDeltaSeconds(), 1, false);
}

void RemoteControllerServer::HandleInputReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Sender)
{
	FUdpRemoteControllerSegment::FHeaderChunk Chunk;
	*Data << Chunk;

	switch (Chunk.SegmentType)
	{
	case EUdpRemoteControllerSegment::KeyboardInput:
		ProcessKeyboardInput(Data);
		break;
	case EUdpRemoteControllerSegment::MouseInput:
		ProcessMouseInput(Data);
		break;
	}
}