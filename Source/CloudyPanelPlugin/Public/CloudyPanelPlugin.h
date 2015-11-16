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
	* @param Command The command to be executed
	* @param ControllerId The Controller Id on which command should be executed
	*/
	bool CCloudyPanelPluginModule::ExecuteCommand(FString Command, int32 ControllerId);

	/**
	* Handles input passed by TCP listener
	*
	* @param ConnectionSocket The TCP socket connecting the listener and client
	* @param Endpoint The endpoint of the socket connection
	*/
	bool CCloudyPanelPluginModule::InputHandler(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint);


	/** Timer */

	/**
	* Timer to poll for input from client, and call appropriate input handler
	*
	* @param DeltaTime The time between polls
	*/
	bool CCloudyPanelPluginModule::Tick(float DeltaTime);


	/** Helper Methods*/

	/**
	* Helper method to send message to client
	*
	* @param Socket The TCP socket used to send the message
	* @param Msg The message to be sent
	*/
	bool CCloudyPanelPluginModule::SendToClient(FSocket* Socket, FString Msg);

	//Rama's String From Binary Array
	//This function requires #include <string>
	FString CCloudyPanelPluginModule::StringFromBinaryArray(const TArray<uint8>& BinaryArray);


	/** Class Variables */

	FSocket* TCPConnection;
	FString InputStr;
	bool HasInputStrChanged;

};