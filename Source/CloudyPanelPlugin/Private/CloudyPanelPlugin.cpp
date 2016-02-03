// Some copyright should be here...
/*=============================================================================
	CloudPanelPlugin.cpp: Implementation of CloudyPanel TCP Plugin
=============================================================================*/
//TCP implementation from : https ://wiki.unrealengine.com/TCP_Socket_Listener,_Receive_Binary_Data_From_an_IP/Port_Into_UE4,_(Full_Code_Sample)


#include "CloudyPanelPluginPrivatePCH.h"
#include "CloudyPanelPlugin.h"

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#define LOCTEXT_NAMESPACE "CCloudyPanelPluginModule"

DEFINE_LOG_CATEGORY(ModuleLog)

#define SERVER_NAME "Listener"
#define SERVER_ENDPOINT FIPv4Endpoint(FIPv4Address(127, 0, 0, 1), 55556)
#define BUFFER_SIZE 1024
#define CONNECTION_THREAD_TIME 1 // in seconds
#define FPS 5 // frames per second
#define SUCCESS_MSG "Success"
#define FAILURE_MSG "Failure"
#define PIXEL_SIZE 4

void CCloudyPanelPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Set up to receive commands from CloudyWeb/CloudyPanel
	// Start timer function to check on client connection
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &CCloudyPanelPluginModule::Tick), CONNECTION_THREAD_TIME);

	// start the server (listener)

	//Create Socket
	FIPv4Endpoint Endpoint(SERVER_ENDPOINT);
	FSocket* ListenSocket = FTcpSocketBuilder(SERVER_NAME).AsReusable().BoundToEndpoint(Endpoint).Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(BUFFER_SIZE, NewSize);

	FTcpListener* TcpListener = new FTcpListener(*ListenSocket, CONNECTION_THREAD_TIME);
	TcpListener->OnConnectionAccepted().BindRaw(this, &CCloudyPanelPluginModule::InputHandler);

	// initialise class variables
	InputStr = "";
	HasInputStrChanged = false;
	isEngineRunning = false;

	// timer to capture frames
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &CCloudyPanelPluginModule::CaptureFrame), 1.0 / FPS);
	
}



void CCloudyPanelPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.	
	
}

bool CCloudyPanelPluginModule::Tick(float DeltaTime)
{
	bool Success = false;

	if (HasInputStrChanged) {

		if (GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread())
		{

			// Split input into Command and ControllerId
			FString CommandStr = InputStr.Mid(0, 4);
			FString ControllerIdStr = InputStr.Mid(4, 4);
			int32 Command = FCString::Atoi(*CommandStr);
			int32 ControllerId = FCString::Atoi(*ControllerIdStr);
			
			//UE_LOG(ModuleLog, Warning, TEXT("Command: %d ControllerId: %d"), Command, ControllerId);
			
			Success = ExecuteCommand(Command, ControllerId);

			InputStr = "";
			HasInputStrChanged = false;
		}

		// Send response to client
		if (Success) {
			SendToClient(TCPConnection, SUCCESS_MSG);
		}
		else {
			SendToClient(TCPConnection, FAILURE_MSG);
		}

	}

	return true;
}



bool CCloudyPanelPluginModule::SendToClient(FSocket* Socket, FString Msg)
{
	TCHAR *serialisedChar = Msg.GetCharArray().GetData();
	int32 size = FCString::Strlen(serialisedChar);
	int32 sent = 0;
	return Socket->Send((uint8*)TCHAR_TO_UTF8(serialisedChar), size, sent);

	return true;
}

//Rama's String From Binary Array
//This function requires #include <string>
FString CCloudyPanelPluginModule::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}

bool CCloudyPanelPluginModule::InputHandler(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint) {

	TArray<uint8> ReceivedData;
	uint32 Size;

	// wait for data to arrive
	while (!(ConnectionSocket->HasPendingData(Size)));

	// handle data - change current command
	ReceivedData.Init(FMath::Min(Size, 65507u));

	int32 Read = 0;
	ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
	FString ReceivedString = StringFromBinaryArray(ReceivedData);
	// UE_LOG(ModuleLog, Warning, TEXT("Data: %s"), *ReceivedString);

	InputStr = ReceivedString;
	HasInputStrChanged = true;

	TCPConnection = ConnectionSocket;

	return true;

}

bool CCloudyPanelPluginModule::ExecuteCommand(int32 Command, int32 ControllerId) {
	bool Success = false;
	if (GEngine)
	{
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

	return Success;
}

void CCloudyPanelPluginModule::SetUpVideoCapture() {

	// get viewport and split screen data
	FViewport* ReadingViewport = GEngine->GameViewport->Viewport;

	ULocalPlayer* FirstPlayer = GEngine->FindFirstLocalPlayerFromControllerId(0);
	TArray<FSplitscreenData> SplitscreenInfo = FirstPlayer->ViewportClient->SplitscreenInfo;
	int SplitscreenType = FirstPlayer->ViewportClient->GetCurrentSplitscreenConfiguration();

	// use VideoPipe (class variable) to pass frames to encoder
	sizeX = ReadingViewport->GetSizeXY().X;
	sizeY = ReadingViewport->GetSizeXY().Y;

	// odd frame height - for frame offset
	if (sizeY % 2 == 1) {
		isOddFrameHeight = true;
		UE_LOG(ModuleLog, Warning, TEXT("Odd Height!!"));
	}
	else {
		isOddFrameHeight = false;
		UE_LOG(ModuleLog, Warning, TEXT("Even Height!!"));
	}

	// assume 2 player
	sizeY = sizeY / 2;

	//UE_LOG(ModuleLog, Warning, TEXT("Width: %d Height: %d"), sizeX, sizeY);
	std::stringstream sstm1;
	std::stringstream sstm2;

	/*
	// write to rtp stream
	sstm1 << "ffmpeg -y -loglevel verbose -re " << " -f rawvideo -pix_fmt rgba -s " << sizeX << "x" << sizeY << " -r " << FPS << " -i - -c:v libx264 -preset ultrafast -an -f rtp rtp://127.0.0.1:1234 -r " << FPS << " 2> out.txt";
	VideoPipe1 = _popen(sstm1.str().c_str(), "wb"); // write as binary

	sstm2 << "ffmpeg -y -loglevel verbose -re " << " -f rawvideo -pix_fmt rgba -s " << sizeX << "x" << sizeY << " -r " << FPS << " -i - -c:v libx264 -preset ultrafast -an -f rtp rtp://127.0.0.1:1235 -r " << FPS << " 2> out.txt";
	VideoPipe2 = _popen(sstm2.str().c_str(), "wb"); // write as binary
	*/

	// write to file
	sstm1 << "ffmpeg -y -loglevel verbose -re " << "-f rawvideo -pix_fmt rgba -s " << sizeX << "x" << sizeY << " -r " << FPS << " -i - -an -f avi output1.avi -r " << FPS << " 2> out1.txt";
	VideoPipe1 = _popen(sstm1.str().c_str(), "wb"); // write as binary

	sstm2 << "ffmpeg -y -loglevel verbose -re " << "-f rawvideo -pix_fmt rgba -s " << sizeX << "x" << sizeY << " -r " << FPS << " -i - -an -f avi output2.avi -r " << FPS << " 2> out2.txt";
	VideoPipe2 = _popen(sstm2.str().c_str(), "wb"); // write as binary

}


bool CCloudyPanelPluginModule::CaptureFrame(float DeltaTime) {
	//UE_LOG(ModuleLog, Warning, TEXT("time %f"), DeltaTime); // can track running time

	// engine has been started
	if (!isEngineRunning && GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread()) {
		isEngineRunning = true;
		CCloudyPanelPluginModule::SetUpVideoCapture();
		UE_LOG(ModuleLog, Warning, TEXT("engine started"));
	}

	// engine has been stopped
	else if (isEngineRunning && !(GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread())) {
		isEngineRunning = false;
		UE_LOG(ModuleLog, Warning, TEXT("engine stopped"));
		fflush(VideoPipe1);
		fflush(VideoPipe2);
		fclose(VideoPipe1);
		fclose(VideoPipe2);
	}

	if (GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread())
	{
		// get viewport and split screen data
		FViewport* ReadingViewport = GEngine->GameViewport->Viewport;
		ReadingViewport->ReadPixels(FrameBuffer);
		ULocalPlayer* FirstPlayer = GEngine->FindFirstLocalPlayerFromControllerId(0);
		TArray<FSplitscreenData> SplitscreenInfo = FirstPlayer->ViewportClient->SplitscreenInfo;
		int SplitscreenType = FirstPlayer->ViewportClient->GetCurrentSplitscreenConfiguration();

		// split screen for 2 players horizontal
	
		if (SplitscreenType == ESplitScreenType::TwoPlayer_Horizontal) {
			Split2Player(FrameBuffer);
		}

		CCloudyPanelPluginModule::StreamFrameToClient();
	}
	return true;
}

void CCloudyPanelPluginModule::StreamFrameToClient() {

	
	// use VideoPipe (class variable) to pass frames to encoder
	uint32 *PixelBuffer;

	// Player 1
	PixelBuffer = new uint32[sizeX * sizeY * PIXEL_SIZE];

	int i = 0;
	for (auto& Pixel : FrameBuffer1) {
		std::ostringstream PixelStream;
		PixelBuffer[i] = Pixel.A * 256 * 256 * 256 + Pixel.B * 256 * 256 + Pixel.G * 256 + Pixel.R;
		// equivalent function using bitshift - can compare performance later
		// PixelBuffer[i] = Pixel.A << 24 | Pixel.B << 16 | Pixel.G << 8 | Pixel.R;
		i++;
	}

	fwrite(PixelBuffer, sizeX * PIXEL_SIZE, sizeY, VideoPipe1);

	delete[]PixelBuffer;

	//Player 2
	PixelBuffer = new uint32[sizeX * sizeY * PIXEL_SIZE];

	i = 0;
	for (auto& Pixel : FrameBuffer2) {
		std::ostringstream PixelStream;
		PixelBuffer[i] = Pixel.A * 256 * 256 * 256 + Pixel.B * 256 * 256 + Pixel.G * 256 + Pixel.R;
		// equivalent function using bitshift - can compare performance later
		// PixelBuffer[i] = Pixel.A << 24 | Pixel.B << 16 | Pixel.G << 8 | Pixel.R;
		i++;
	}

	fwrite(PixelBuffer, sizeX * PIXEL_SIZE, sizeY, VideoPipe2);

	delete[]PixelBuffer;
}

// Split screen for 2 player
void CCloudyPanelPluginModule::Split2Player(TArray<FColor> FrameBuffer) {

	// empty buffers first
	FrameBuffer1.Empty();
	FrameBuffer2.Empty();

	// Player 1
	for (int i = 0; i < FrameBuffer.Num() / 2; i++) {
		FrameBuffer1.Add(FrameBuffer[i]);
	}

	// Player 2 - deal with odd frame height
	int StartingPixel;
	if (isOddFrameHeight) {
		StartingPixel = FrameBuffer.Num() / 2 + sizeX / 2;
	}
	else {
		StartingPixel = FrameBuffer.Num() / 2;
	}

	for (int i = StartingPixel; i < FrameBuffer.Num(); i++) {
		FrameBuffer2.Add(FrameBuffer[i]);
	}

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(CCloudyPanelPluginModule, CloudyPanelPlugin)