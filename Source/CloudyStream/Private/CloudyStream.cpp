// Some copyright should be here...

#include "CloudyStreamPrivatePCH.h"

#include <stdio.h>
#include <sstream>

#define LOCTEXT_NAMESPACE "CloudyStream"

#define PIXEL_SIZE 4
#define FPS 30 // frames per second

// Layout of the split screen. Ensure that this is same as the values in GameViewportClient.cpp line 120~
#define MAX_NUM_PLAYERS 6
#define NUM_ROWS 2
#define NUM_COLS 3

DEFINE_LOG_CATEGORY(CloudyStreamLog)

void CloudyStreamImpl::StartupModule()
{
    ensure(MAX_NUM_PLAYERS == NUM_ROWS * NUM_COLS);
	// timer to capture frames
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &CloudyStreamImpl::CaptureFrame), 1.0 / FPS);
	UE_LOG(CloudyStreamLog, Warning, TEXT("Streaming module started"));

}


void CloudyStreamImpl::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

}


// call this for each player join
void CloudyStreamImpl::SetUpPlayer(int ControllerId, int StreamingPort, FString StreamingIP) {

	// encode and write players' frames to http stream
	std::stringstream *StringStream = new std::stringstream();

	std::string StreamingIPString(TCHAR_TO_UTF8(*StreamingIP)); // convert to std::string
	
	*StringStream << "ffmpeg -y " << " -f rawvideo -pix_fmt bgra -s " << halfSizeX << "x" << halfSizeY << " -r " << FPS << " -i - -listen 1 -c:v libx264 -preset slow -f avi -an -tune zerolatency http://" << StreamingIPString << ":" << StreamingPort;
	VideoPipeList.Add(_popen(StringStream->str().c_str(), "wb"));

	// add frame buffer for new player
	TArray<FColor> TempFrameBuffer;
	FrameBufferList.Add(TempFrameBuffer);

	PlayerFrameMapping.Add(ControllerId);
	NumberOfPlayers++;
}


void CloudyStreamImpl::SetUpVideoCapture() {

	// init frame dimension variables
	FViewport* ReadingViewport = GEngine->GameViewport->Viewport;
	sizeX = ReadingViewport->GetSizeXY().X;
	sizeY = ReadingViewport->GetSizeXY().Y;

    UE_LOG(CloudyStreamLog, Warning, TEXT("Height: %d Width: %d"), sizeY, sizeX);

    float RowIncrement = sizeY / NUM_ROWS;
    float ColIncrement = sizeX / NUM_COLS;

    halfSizeX = sizeX / NUM_COLS;
    halfSizeY = sizeY / NUM_ROWS;

    for (float i = 0.0f; i < sizeY; i += RowIncrement)
    {
        for (float k = 0.0f; k < sizeX; k += ColIncrement)
        {
            // FIntRect(TopLeftX, TopLeftY, BottomRightX, BottomRightY)
            ScreenList.Add(FIntRect(k, i, k + ColIncrement, i + RowIncrement));
            UE_LOG(CloudyStreamLog, Warning, TEXT("Iteration: i: %f k: %f"), k, i);
        }
    }

    //ScreenList.Add(FIntRect(0, 0, ColIncrement, RowIncrement)); // 1
    //ScreenList.Add(FIntRect(ColIncrement, 0, ColIncrement*2, RowIncrement)); // 2
    //ScreenList.Add(FIntRect(ColIncrement*2, 0, ColIncrement*3, RowIncrement)); // 3
    //
    //ScreenList.Add(FIntRect(0, RowIncrement, ColIncrement, RowIncrement*2)); // 4
    //ScreenList.Add(FIntRect(ColIncrement, RowIncrement, ColIncrement*2, RowIncrement*2)); // 5
    //ScreenList.Add(FIntRect(ColIncrement*2, RowIncrement, ColIncrement*3, RowIncrement*2)); // 6
	
	
	// set up split screen info
	//Screen1 = FIntRect(0, 0, halfSizeX, halfSizeY);
	//Screen2 = FIntRect(halfSizeX, 0, sizeX, halfSizeY);
	//Screen3 = FIntRect(0, halfSizeY, halfSizeX, sizeY);
	//Screen4 = FIntRect(halfSizeX, halfSizeY, sizeX, sizeY);
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

		for (int i = 0; i < NumberOfPlayers; i++) {
			// flush and close video pipes
			fflush(VideoPipeList[i]);
			fclose(VideoPipeList[i]);
		}
	}

	if (GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread())
	{
		// split screen for 4 players
		Split4Player();
		StreamFrameToClient();
	}
	return true;
}


void CloudyStreamImpl::StreamFrameToClient() {

	// use VideoPipe (class variable) to pass frames to encoder
	uint32 *PixelBuffer;
	FColor Pixel;
	PixelBuffer = new uint32[sizeX * sizeY * PIXEL_SIZE];

	for (int i = 0; i < NumberOfPlayers; i++) {
		int FrameSize = FrameBufferList[i].Num();

		for (int j = 0; j < FrameSize; ++j) {
			Pixel = FrameBufferList[PlayerFrameMapping[i]][j];
			PixelBuffer[j] = Pixel.DWColor();
		}

		fwrite(PixelBuffer, halfSizeX * PIXEL_SIZE, halfSizeY, VideoPipeList[i]);
	}
	delete[]PixelBuffer;
}


// Split screen for 4 player
void CloudyStreamImpl::Split4Player() {

	FViewport* ReadingViewport = GEngine->GameViewport->Viewport;

   //for (int i = 0; i < NumberOfPlayers; i++)
   //{
   //    ReadingViewport->ReadPixels(FrameBufferList[i], flags, ScreenList[i]);
   //}

    if (NumberOfPlayers > 0)
        ReadingViewport->ReadPixels(FrameBufferList[0], flags, ScreenList[0]);
	if (NumberOfPlayers > 1)
        ReadingViewport->ReadPixels(FrameBufferList[1], flags, ScreenList[1]);
	if (NumberOfPlayers > 2)
        ReadingViewport->ReadPixels(FrameBufferList[2], flags, ScreenList[2]);
	if (NumberOfPlayers > 3)
        ReadingViewport->ReadPixels(FrameBufferList[3], flags, ScreenList[3]);

}


void CloudyStreamImpl::StartPlayerStream(int32 ControllerId, int32 StreamingPort, FString StreamingIP)
{
	UE_LOG(CloudyStreamLog, Warning, TEXT("Player %d stream started"), ControllerId);
	SetUpPlayer(ControllerId, StreamingPort, StreamingIP);	
}


void CloudyStreamImpl::StopPlayerStream(int32 ControllerId)
{
	UE_LOG(CloudyStreamLog, Warning, TEXT("Player %d stream stopped"), ControllerId);
	int PipeIndex = PlayerFrameMapping.Find(ControllerId);
	fflush(VideoPipeList[PipeIndex]);
	fclose(VideoPipeList[PipeIndex]);
	PlayerFrameMapping.Remove(ControllerId);
	VideoPipeList.RemoveAt(PipeIndex);
	NumberOfPlayers--;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(CloudyStreamImpl, CloudyStream)