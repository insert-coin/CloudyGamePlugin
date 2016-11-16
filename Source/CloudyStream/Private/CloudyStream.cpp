// Some copyright should be here...

#include "CloudyStreamPrivatePCH.h"

#define LOCTEXT_NAMESPACE "CloudyStream"

#define CONNECTION_THREAD_TIME 5 // in seconds

// Layout of the split screen. Ensure that this is same as the values in GameViewportClient.cpp line 120~
#define MAX_NUM_PLAYERS 12
#define NUM_ROWS 3.0
#define NUM_COLS 4.0


DEFINE_LOG_CATEGORY(CloudyStreamLog)

void CloudyStreamImpl::StartupModule()
{
    ensure(MAX_NUM_PLAYERS == NUM_ROWS * NUM_COLS);

	// timer to capture frames
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &CloudyStreamImpl::CaptureFrame), CONNECTION_THREAD_TIME);
	UE_LOG(CloudyStreamLog, Warning, TEXT("Streaming module started"));

}


void CloudyStreamImpl::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

}

void CloudyStreamImpl::SetUpVideoCapture() {

    ScreenList.Empty();

	// init frame dimension variables
	FViewport* ReadingViewport = GEngine->GameViewport->Viewport;
	sizeX = ReadingViewport->GetSizeXY().X;
	sizeY = ReadingViewport->GetSizeXY().Y;

    UE_LOG(CloudyStreamLog, Warning, TEXT("Height: %d Width: %d"), sizeY, sizeX);

    RowIncrement = sizeY / NUM_ROWS;
    ColIncrement = sizeX / NUM_COLS;

    RowIncInt = sizeY / (int)NUM_ROWS;
    ColIncInt = sizeX / (int)NUM_COLS;

    UE_LOG(CloudyStreamLog, Warning, TEXT("RowIncrement: %f ColIncrement: %f"), RowIncrement, ColIncrement);

    for (float i = 0.0f; i < sizeY; i += RowIncrement)
    {
        for (float k = 0.0f; k < sizeX; k += ColIncrement)
        {
            ScreenList.Add(FIntRect(k, i, k + ColIncInt, i + RowIncInt));
        }
    }

	flags = FReadSurfaceDataFlags(ERangeCompressionMode::RCM_MinMaxNorm, ECubeFace::CubeFace_NegX);

}


bool CloudyStreamImpl::CaptureFrame(float DeltaTime) {

	// engine has been started
	if (!isEngineRunning && GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread()) {
		isEngineRunning = true;
		SetUpVideoCapture();
		UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
		NumberOfPlayers = 0;
		
		ULocalPlayer* const ExistingPlayer = GameInstance->FindLocalPlayerFromControllerId(0);
		APlayerController* Controller = ExistingPlayer->PlayerController;
		Controller->GetPawn()->Destroy();
		GameInstance->DebugRemovePlayer(0); // remove default first player
	}

	// engine has been stopped
	else if (isEngineRunning && !(GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread())) {
		isEngineRunning = false;
	}

	return true;
}


void CloudyStreamImpl::StartPlayerStream(int32 ControllerId)
{
	UE_LOG(CloudyStreamLog, Warning, TEXT("Player %d stream started"), ControllerId);
    NumberOfPlayers++;
}


void CloudyStreamImpl::StopPlayerStream(int32 ControllerId)
{
	UE_LOG(CloudyStreamLog, Warning, TEXT("Player %d stream stopped"), ControllerId);
	NumberOfPlayers--;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(CloudyStreamImpl, CloudyStream)