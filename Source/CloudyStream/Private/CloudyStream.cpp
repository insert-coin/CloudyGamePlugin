// Some copyright should be here...

#include "CloudyStreamPrivatePCH.h"

#include <stdio.h>
#include <sstream>

#define LOCTEXT_NAMESPACE "CloudyStream"

#define PIXEL_SIZE 4
#define FPS 30 // frames per second


DEFINE_LOG_CATEGORY(CloudyStreamLog)

void CloudyStreamImpl::StartupModule()
{

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
	
	*StringStream << "ffmpeg -y " << " -f rawvideo -pix_fmt rgba -s " << halfSizeX << "x" << halfSizeY << " -r " << FPS << " -i - -listen 1 -c:v libx264 -preset slow -f avi -an -tune zerolatency http://" << StreamingIPString << ":" << StreamingPort << " 2> out" << ControllerId << ".txt";
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
	halfSizeX = sizeX / 2;
	halfSizeY = sizeY / 2;
	UE_LOG(CloudyStreamLog, Warning, TEXT("Height: %d Width: %d"), sizeY, sizeX);

	// set up split screen info
	Screen1 = FIntRect(0, 0, halfSizeX, halfSizeY);
	Screen2 = FIntRect(halfSizeX, 0, sizeX, halfSizeY);
	Screen3 = FIntRect(0, halfSizeY, halfSizeX, sizeY);
	Screen4 = FIntRect(halfSizeX, halfSizeY, sizeX, sizeY);
	flags = FReadSurfaceDataFlags(ERangeCompressionMode::RCM_MinMaxNorm, ECubeFace::CubeFace_NegX);

}


bool CloudyStreamImpl::CaptureFrame(float DeltaTime) {
	//UE_LOG(CloudyStreamLog, Warning, TEXT("time %f"), DeltaTime); // can track running time


	// engine has been started
	if (!isEngineRunning && GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread()) {
		isEngineRunning = true;
		SetUpVideoCapture();
		UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
		NumberOfPlayers = 0;
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
			PixelBuffer[j] = Pixel.A << 24 | Pixel.B << 16 | Pixel.G << 8 | Pixel.R;
		}

		fwrite(PixelBuffer, halfSizeX * PIXEL_SIZE, halfSizeY, VideoPipeList[i]);
	}
	delete[]PixelBuffer;
}


// Split screen for 4 player
void CloudyStreamImpl::Split4Player() {

	FViewport* ReadingViewport = GEngine->GameViewport->Viewport;

	if (NumberOfPlayers > 0)
		ReadingViewport->ReadPixels(FrameBufferList[0], flags, Screen1);
	if (NumberOfPlayers > 1)
		ReadingViewport->ReadPixels(FrameBufferList[1], flags, Screen2);
	if (NumberOfPlayers > 2)
		ReadingViewport->ReadPixels(FrameBufferList[2], flags, Screen3);
	if (NumberOfPlayers > 3)
		ReadingViewport->ReadPixels(FrameBufferList[3], flags, Screen4);

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