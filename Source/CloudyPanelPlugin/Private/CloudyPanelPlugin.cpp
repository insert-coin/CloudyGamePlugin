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

void CCloudyPanelPluginModule::SetUpVideoCapture() {
	
	// get viewport and split screen data
	FViewport* ReadingViewport = GEngine->GameViewport->Viewport;
		
	ULocalPlayer* FirstPlayer = GEngine->FindFirstLocalPlayerFromControllerId(0);
	TArray<FSplitscreenData> SplitscreenInfo = FirstPlayer->ViewportClient->SplitscreenInfo;
	int SplitscreenType = FirstPlayer->ViewportClient->GetCurrentSplitscreenConfiguration();

	// use VideoPipe (class variable) to pass frames to encoder
	int sizeX = ReadingViewport->GetSizeXY().X;
	int sizeY = ReadingViewport->GetSizeXY().Y;

	//UE_LOG(ModuleLog, Warning, TEXT("Width: %d Height: %d"), sizeX, sizeY);
	std::stringstream sstm;
	sstm << "ffmpeg -y -loglevel verbose -re " << "-f rawvideo -pix_fmt rgba -s " << sizeX << "x" << sizeY << " -r " << FPS << " -i - -an -f rtp rtp://127.0.0.1:1234 -r " << FPS << " 2> out.txt";
	VideoPipe = _popen(sstm.str().c_str(), "wb"); // write as binary

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


bool CCloudyPanelPluginModule::CaptureFrame(float DeltaTime) {
	//UE_LOG(ModuleLog, Warning, TEXT("time %f"), DeltaTime); // can track running time
	
	if (!isEngineRunning && GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread()) { // engine has been started
		isEngineRunning = true;
		CCloudyPanelPluginModule::SetUpVideoCapture();
		UE_LOG(ModuleLog, Warning, TEXT("engine started"));
	}
	else if (isEngineRunning && !(GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread())) { // engine has been stopped
		isEngineRunning = false;
		UE_LOG(ModuleLog, Warning, TEXT("engine stopped"));
		
		fclose(VideoPipe);
	}

	if (GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread())
	{
		
		// init buffers for each player, up to 4
		TArray<FColor> OutBuffer, OutBuffer1, OutBuffer2, OutBuffer3, OutBuffer4;
		
		// get viewport and split screen data
		FViewport* ReadingViewport = GEngine->GameViewport->Viewport;
		ReadingViewport->ReadPixels(FrameBuffer);
		ULocalPlayer* FirstPlayer = GEngine->FindFirstLocalPlayerFromControllerId(0);
		TArray<FSplitscreenData> SplitscreenInfo = FirstPlayer->ViewportClient->SplitscreenInfo;
		int SplitscreenType = FirstPlayer->ViewportClient->GetCurrentSplitscreenConfiguration();

		// use VideoPipe (class variable) to pass frames to encoder
		int sizeX = ReadingViewport->GetSizeXY().X;
		int sizeY = ReadingViewport->GetSizeXY().Y;
		uint32 *PixelBuffer;
		PixelBuffer = new uint32[sizeX * sizeY * PIXEL_SIZE];
		
		int i = 0;
		for (auto& Pixel : FrameBuffer) {
			std::ostringstream PixelStream;
			PixelBuffer[i] = Pixel.A * 256 * 256 * 256 + Pixel.B * 256 * 256 + Pixel.G * 256 + Pixel.R;
			// equivalent function using bitshift - can compare performance later
			// PixelBuffer[i] = Pixel.A << 24 | Pixel.B << 16 | Pixel.G << 8 | Pixel.R;
			i++;
		}

		fwrite(PixelBuffer, sizeX * PIXEL_SIZE, sizeY, VideoPipe);
		
		fflush(VideoPipe);

		delete []PixelBuffer;
		
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

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(CCloudyPanelPluginModule, CloudyPanelPlugin)