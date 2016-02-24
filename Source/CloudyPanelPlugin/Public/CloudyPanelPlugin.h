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
	* Timer to poll for input from client, and call appropriate input handler
	*
	* @param DeltaTime The time between polls
	*/
	bool CCloudyPanelPluginModule::Tick(float DeltaTime);

	// Timer for capturing frames
	bool CCloudyPanelPluginModule::CaptureFrame(float DeltaTime);


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

	/** Video Capture*/
	void CCloudyPanelPluginModule::SetUpVideoCapture();
	void CCloudyPanelPluginModule::SetUpPlayer(int ControllerId);
	void CCloudyPanelPluginModule::StreamFrameToClient();
	// Only handle 4 player split screen for current solution
	void CCloudyPanelPluginModule::Split4Player();


	/** Class Variables */

	// For TCP listener
	FSocket* TCPConnection;
	FString InputStr;
	bool HasInputStrChanged;

	// For Video capture
	int NumberOfPlayers;
	TArray<FILE*> VideoPipeList;
	TArray<TArray<FColor> > FrameBufferList;
	bool isEngineRunning;
	int sizeX, sizeY, halfSizeX, halfSizeY;
	TArray<int> PlayerFrameMapping; // index is frame index, value is player index
	FIntRect Screen1, Screen2, Screen3, Screen4;
	FReadSurfaceDataFlags flags;

	/**
	* Enum to establish command protocol between CloudyPanel and CloudyPanelPlugin
	*/
	enum CommandList {
		JOIN_GAME,
		QUIT_GAME
	};

};