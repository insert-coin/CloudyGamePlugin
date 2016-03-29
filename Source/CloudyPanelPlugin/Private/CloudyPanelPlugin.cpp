// Some copyright should be here...
/*=============================================================================
CloudPanelPlugin.cpp: Implementation of CloudyPanel TCP Plugin
=============================================================================*/
//TCP implementation from : https ://wiki.unrealengine.com/TCP_Socket_Listener,_Receive_Binary_Data_From_an_IP/Port_Into_UE4,_(Full_Code_Sample)

#include "CloudyPanelPluginPrivatePCH.h"
#include "CloudyPanelPlugin.h"
#include "../../CloudyStream/Public/CloudyStream.h"
#include "../../CloudyWebAPI/Public/ICloudyWebAPI.h"


#include <stdio.h>
#include <stdlib.h>
#include <string>

#define LOCTEXT_NAMESPACE "CCloudyPanelPluginModule"

DEFINE_LOG_CATEGORY(ModuleLog)


#define DELETE_URL "/game-session/"
#define DELETE_REQUEST "DELETE"
#define MAX_PLAYERS 4


void CCloudyPanelPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UE_LOG(ModuleLog, Warning, TEXT("CloudyPanel Plugin started"));

	// initialise game session id mapping
	int GameSessionIdMapping[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		GameSessionIdMapping[i] = -1;
	}
}

void CCloudyPanelPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.	
	

}


bool CCloudyPanelPluginModule::ExecuteCommand(FString Command,
	int32 ControllerId, int32 StreamingPort, FString StreamingIP, int32 GameSessionId)
{
	if (Command == "join")
	{
		return AddPlayer(ControllerId, StreamingPort, StreamingIP, GameSessionId);
	}
	else if (Command == "quit")
	{
		return RemovePlayer(ControllerId, GameSessionId);
	}
	else
	{
		return false;
	}
}


bool CCloudyPanelPluginModule::AddPlayer(int32 ControllerId, int32 StreamingPort,
	FString StreamingIP, int32 GameSessionId)
{
	UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
	FString Error;
	GameInstance->CreateLocalPlayer(ControllerId, Error, true);

	if (Error.Len() == 0) // success. no error message
	{
		CloudyStreamImpl::Get().StartPlayerStream(ControllerId, StreamingPort, StreamingIP);
		GameSessionIdMapping[ControllerId] = GameSessionId;
		return true;
	}

	return false;

}


bool CCloudyPanelPluginModule::RemovePlayer(int32 ControllerId, int32 GameSessionId)
{
	UGameInstance* GameInstance = GEngine->GameViewport->GetGameInstance();
	ULocalPlayer* const ExistingPlayer = GameInstance->FindLocalPlayerFromControllerId(ControllerId);
	bool Success = false;

	if (ExistingPlayer != NULL)
	{
		UE_LOG(ModuleLog, Warning, TEXT("Controller Id: %d"), ControllerId);

		// Future implementation of Quit Game - delete session from server

		// delete appropriate game session
		int32 GameSessionId = GameSessionIdMapping[ControllerId];
		FString GameSessionString = DELETE_URL + FString::FromInt(GameSessionId) + "/";
		UE_LOG(ModuleLog, Warning, TEXT("Game Session string: %s"), *GameSessionString);
		Success = ICloudyWebAPI::Get().MakeRequest(GameSessionString, DELETE_REQUEST);

		// check for successful removal from server before removing
		if (Success)
		{
			CloudyStreamImpl::Get().StopPlayerStream(ControllerId);
			GameSessionIdMapping[ControllerId] = -1;
			return GameInstance->RemoveLocalPlayer(ExistingPlayer);
		}
	}

	return false;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(CCloudyPanelPluginModule, CloudyPanelPlugin)