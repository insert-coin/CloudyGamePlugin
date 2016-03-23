// Some copyright should be here...

#pragma once

#include "ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(ModuleLog, Log, All)

class CCloudyPanelPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Input Handlers*/

	/**
	* Executes commands such as add and remove player on the gamethread
	*
	* @param Command The command to be executed (enumerated)
	* @param ControllerId The Controller Id on which command should be executed
	*/
	/*=============================================================================
	Command Format
	Incoming String: [01234567]
	[0123] - Command (enumerated)
	[4567] - Controller ID
	=============================================================================*/
	bool CCloudyPanelPluginModule::ExecuteCommand(int32 Command, int32 ControllerId);

	/**
	* Handles input passed by TCP listener
	*
	* @param ConnectionSocket The TCP socket connecting the listener and client
	* @param Endpoint The endpoint of the socket connection
	*/
	bool CCloudyPanelPluginModule::InputHandler(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint);


	/** Timer */

	/**
	* Timer to poll for input from client, and call appropriate input handler.
	* Stops when input is received.
	*
	* @param DeltaTime The time between polls
	*/
	bool CCloudyPanelPluginModule::CheckConnection(float DeltaTime);


	/** Helper Methods*/

	/**
	* Helper method to send message to client
	*
	* @param Socket The TCP socket used to send the message
	* @param Msg The message to be sent
	*/
	bool CCloudyPanelPluginModule::SendToClient(FSocket* Socket, FString Msg);

	/**
	* Method to add player to the game by Controller ID.
	*
	* @param ControllerId The Controller ID of the player to be added
	*/
	bool CCloudyPanelPluginModule::AddPlayer(int32 ControllerId);

	/**
	* Method to remove player by Controller ID. Sends a delete game session
	* request to CloudyWeb server
	*
	* @param ControllerId The Controller ID of the player to be deleted
	*/
	bool CCloudyPanelPluginModule::RemovePlayer(int32 ControllerId);

	

	//Rama's String From Binary Array
	//This function requires #include <string>
	FString CCloudyPanelPluginModule::StringFromBinaryArray(const TArray<uint8>& BinaryArray);


	/** Class Variables */

	FSocket* ListenSocket;
	FSocket* TCPConnection;
	FTcpListener* TcpListener;
	FString InputStr;
	bool HasInputStrChanged;

	/**
	* Enum to establish command protocol between CloudyPanel and CloudyPanelPlugin
	*/
	enum CommandList {
		JOIN_GAME,
		QUIT_GAME
	};


};