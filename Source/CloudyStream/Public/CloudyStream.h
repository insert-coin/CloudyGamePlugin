// Some copyright should be here...

#pragma once

#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(CloudyStreamLog, Log, All)

class CloudyStreamImpl : public IModuleInterface
{
public:

	/** Methods **/

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	* One-time set up for variables used during streaming, including frame 
	* dimensions and split-screen information
	*/
	void SetUpVideoCapture();

	/**
	* Sets up variables required per-player for streaming, including the 
	* player's stream and mapping from player's ControllerId to Frame index.
	* This is called whenever a player joins the game.
	*
	* @param ControllerId The Controller ID of the player who joins game
	* @param StreamingPort The port to stream this player from
	* @param StreamingIP The IP to stream this game from
	*/
	void SetUpPlayer(int ControllerId, int StreamingPort, FString StreamingIP);

	/**
	* Handles streaming of the frame to client
	*/
	void StreamFrameToClient();

	/**
	* Handles 4 player split-screen. The split frame buffers are stored in
	* class variables
	*/
	void Split4Player();

	/**
	* Starts a stream for a player. This is called by CloudyPlayerManager
	* (another module). It can be accessed by other modules.
	*
	* @param ControllerId The Controller ID of the player to start streaming
	* @param StreamingPort The port to stream this player from
	* @param StreamingIP The IP to stream this game from
	*/
	virtual void StartPlayerStream(int32 ControllerId, int32 StreamingPort, FString StreamingIP);

	/**
	* Stops a player's stream and clean up. This is called by CloudyPlayerManager
	* (another module). It can be accessed by other modules.
	*
	* @param ControllerId The Controller ID of the player to start streaming
	*/
	virtual void StopPlayerStream(int32 ControllerId);


	/**
	* Singleton-like access to this module's interface.  This is just for 
	* convenience! Beware of calling this during the shutdown phase, though.
	* Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
	static inline CloudyStreamImpl& Get()
	{
		return FModuleManager::LoadModuleChecked< CloudyStreamImpl >("CloudyStream");
	}

	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call
	* Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("CloudyStream");
	}

	
	// Timer for capturing frames
	bool CaptureFrame(float DeltaTime);


	/** Class variables **/
	int NumberOfPlayers;
	TArray<FILE*> VideoPipeList;
	TArray<TArray<FColor> > FrameBufferList;
	bool isEngineRunning;
	int sizeX, sizeY;
    float RowIncrement, ColIncrement;
    int RowIncInt, ColIncInt;
	TArray<int> PlayerFrameMapping; // index is frame index, value is controller ID
    TArray<FIntRect> ScreenList;
	FIntRect Screen1, Screen2, Screen3, Screen4;
	FReadSurfaceDataFlags flags; // needed to read buffer from engine

    

    int OneCounter = 0;
};
