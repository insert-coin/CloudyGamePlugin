// Some copyright should be here...
/*=============================================================================
CloudPanelPlugin.cpp: Implementation of CloudyPanel TCP Plugin
=============================================================================*/
//TCP implementation from : https ://wiki.unrealengine.com/TCP_Socket_Listener,_Receive_Binary_Data_From_an_IP/Port_Into_UE4,_(Full_Code_Sample)

#include "CloudyPanelPluginPrivatePCH.h"
#include "CloudyPanelPlugin.h"
#include "../../CloudyStream/Public/CloudyStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#define LOCTEXT_NAMESPACE "CCloudyPanelPluginModule"

DEFINE_LOG_CATEGORY(ModuleLog)

#define SERVER_NAME "Listener"
#define SERVER_ENDPOINT FIPv4Endpoint(FIPv4Address(127, 0, 0, 1), 55556)
#define CONNECTION_THREAD_TIME 10 // in seconds
#define BUFFER_SIZE 1024

#define SUCCESS_MSG "Success"
#define FAILURE_MSG "Failure"

#define DELETE_URL "/game-session/"
#define DELETE_REQUEST "DELETE"
#define GAME_NAME GInternalGameName


void CCloudyPanelPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Set up to receive commands from CloudyWeb/CloudyPanel
	// Start timer function to check on client connection
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &CCloudyPanelPluginModule::CheckConnection), CONNECTION_THREAD_TIME);

	// start the server (listener)

	//Create Socket
	FIPv4Endpoint Endpoint(SERVER_ENDPOINT);
	ListenSocket = FTcpSocketBuilder(SERVER_NAME).AsReusable().BoundToEndpoint(Endpoint).Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(BUFFER_SIZE, NewSize);

	TcpListener = new FTcpListener(*ListenSocket, CONNECTION_THREAD_TIME);
	TcpListener->OnConnectionAccepted().BindRaw(this, &CCloudyPanelPluginModule::InputHandler);

	// initialise class variables
	InputStr = "";
	HasInputStrChanged = false;

	UE_LOG(ModuleLog, Warning, TEXT("CPP started"));

}


void CCloudyPanelPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.	
	delete TcpListener;
	ListenSocket->Close();

}


bool CCloudyPanelPluginModule::CheckConnection(float DeltaTime)
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
		if (Success)
		{
			SendToClient(TCPConnection, SUCCESS_MSG);
		}
		else
		{
			SendToClient(TCPConnection, FAILURE_MSG);
		}

		return false; // response received. stop timer
	}

	return true; // continue timer to check for reply
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


bool CCloudyPanelPluginModule::InputHandler(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint)
{

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


bool CCloudyPanelPluginModule::ExecuteCommand(int32 Command, int32 ControllerId)
{
	if (GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread())
	{
		switch (Command) {
			case JOIN_GAME:
				return AddPlayer(ControllerId);
			case QUIT_GAME:
				return RemovePlayer(ControllerId);
			default:
				return false;
		}
		return false;
	}
	
	UE_LOG(ModuleLog, Warning, TEXT("Game not started"));
	return false;
}


bool CCloudyPanelPluginModule::AddPlayer(int32 ControllerId)
{
	UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
	FString Error;
	GameInstance->CreateLocalPlayer(ControllerId, Error, true);

	if (Error.Len() == 0) // success. no error message
	{
		CloudyStreamImpl::Get().StartPlayerStream(ControllerId);
		return true;
	}

	return false;

}


bool CCloudyPanelPluginModule::RemovePlayer(int32 ControllerId)
{
	UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
	ULocalPlayer* const ExistingPlayer = GameInstance->FindLocalPlayerFromControllerId(ControllerId);
	if (ExistingPlayer != NULL)
	{

		// Future implementation of Quit Game - delete session from server

		// delete appropriate game session
		/*
		FString Int32String = FString::FromInt(ControllerId);
		FString GameSession = DELETE_URL + Int32String + "/";
		UE_LOG(ModuleLog, Warning, TEXT("Game Session string: %s"), *GameSession);
		Success = ICloudyWebAPI::Get().MakeRequest(Int32String, DELETE_REQUEST);
		*/
		//UE_LOG(ModuleLog, Warning, TEXT("Game name: %s"), GAME_NAME);

		// check for successful removal from server before removing
		// if (Success) {
	
		CloudyStreamImpl::Get().StopPlayerStream(ControllerId);
		return GameInstance->RemoveLocalPlayer(ExistingPlayer);
		
	}

	return false;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(CCloudyPanelPluginModule, CloudyPanelPlugin)