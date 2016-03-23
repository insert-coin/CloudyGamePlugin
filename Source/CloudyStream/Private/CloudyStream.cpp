// Some copyright should be here...

#include "CloudyStreamPrivatePCH.h"

#include <stdio.h>
#include <sstream>

#include "../../CloudyWebAPI/Public/ICloudyWebAPI.h"

#define LOCTEXT_NAMESPACE "CloudyStream"

#define PIXEL_SIZE 4
#define BASE_PORT_NUM 30000
#define FPS 30 // frames per second
#define SEND_REQUEST_TIME 5 // frequency to send HTTP request, in seconds
#define CHECK_RESPONSE_TIME 1 // frequency to check HTTP response, in seconds
#define GAME_URL "/games/?name=" // to access game data from CloudyWeb server
#define GET_REQUEST "GET"
#define GAME_IP_JSON_FIELD "address"


DEFINE_LOG_CATEGORY(CloudyStreamLog)

void CloudyStreamImpl::StartupModule()
{

	// timer to capture frames
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &CloudyStreamImpl::CaptureFrame), 1.0 / FPS);
	UE_LOG(CloudyStreamLog, Warning, TEXT("Streaming module started"));

	// get this game's IP
	GameIP = "";
	// make a request to CloudyWebs server for this game's IP
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &CloudyStreamImpl::RequestGameIP), SEND_REQUEST_TIME);
	// start a timer to receive the response from the request
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &CloudyStreamImpl::GetGameIP), CHECK_RESPONSE_TIME);

}


void CloudyStreamImpl::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

}

bool CloudyStreamImpl::RequestGameIP(float DeltaTime)
{
	if (GameIP == "") // game IP not updated
	{
		FString RequestUrl = GAME_URL + (FString)GInternalGameName;
		ICloudyWebAPI::Get().MakeRequest(RequestUrl, GET_REQUEST);
		return true;
	}
	else
	{	
		return false;
	}
	
}

bool CloudyStreamImpl::GetGameIP(float DeltaTime)
{
	FString Response = ICloudyWebAPI::Get().GetResponse();
	if (Response == "")
	{
		return true; // continue timer to wait for response
	}
	else
	{
		// parse GameIP from JSON response
		
		Response = Response.Replace(TEXT("["), TEXT(""));
		Response = Response.Replace(TEXT("]"), TEXT(""));

		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		TSharedRef<TJsonReader<TCHAR>>JsonReader = TJsonReaderFactory<TCHAR>::Create(Response);
		FJsonSerializer::Deserialize(JsonReader, JsonObject);
		GameIP = JsonObject->GetStringField(GAME_IP_JSON_FIELD);
		//GameIP = Response;

		UE_LOG(CloudyStreamLog, Warning, TEXT("Game IP obtained: %s"), *GameIP);
		return false;
	}
}


// call this for each player join
void CloudyStreamImpl::SetUpPlayer(int ControllerId) {

	// encode and write players' frames to http stream
	std::stringstream *StringStream = new std::stringstream();

	std::string GameIPString(TCHAR_TO_UTF8(*GameIP)); // convert to std::string
	
	*StringStream << "ffmpeg -y " << " -f rawvideo -pix_fmt rgba -s " << halfSizeX << "x" << halfSizeY << " -r " << FPS << " -i - -listen 1 -c:v libx264 -preset slow -f avi -an -tune zerolatency http://" << GameIPString << ":" << BASE_PORT_NUM + ControllerId << " 2> out" << ControllerId << ".txt";
	VideoPipeList.Add(_popen(StringStream->str().c_str(), "wb"));

	// add frame buffer for new player
	TArray<FColor> TempFrameBuffer;
	FrameBufferList.Add(TempFrameBuffer);

	PlayerFrameMapping.Add(ControllerId);

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
		NumberOfPlayers = 1;
		SetUpPlayer(0);


	}

	// engine has been stopped
	else if (isEngineRunning && !(GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread())) {
		
		isEngineRunning = false;

		for (int i = 0; i < NumberOfPlayers; i++) {
			// flush and close video pipes
			int PipeIndex = PlayerFrameMapping[i];
			fflush(VideoPipeList[PipeIndex]);
			fclose(VideoPipeList[PipeIndex]);
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


void CloudyStreamImpl::StartPlayerStream(int32 ControllerId)
{
	UE_LOG(CloudyStreamLog, Warning, TEXT("Player %d stream started"), ControllerId);
	SetUpPlayer(ControllerId);
	NumberOfPlayers++;
}


void CloudyStreamImpl::StopPlayerStream(int32 ControllerId)
{
	UE_LOG(CloudyStreamLog, Warning, TEXT("Player %d stream stopped"), ControllerId);
	int PipeIndex = PlayerFrameMapping[ControllerId];
	fflush(VideoPipeList[PipeIndex]);
	fclose(VideoPipeList[PipeIndex]);
	PlayerFrameMapping.Remove(ControllerId);
	VideoPipeList.RemoveAt(PipeIndex);
	NumberOfPlayers--;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(CloudyStreamImpl, CloudyStream)