// Some copyright should be here...

#pragma once

#include "ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(ModuleLog, Log, All)

class CCloudyPlayerManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Input Handlers*/

	/**
	* Executes commands such as add and remove player on the gamethread
	*
	* @param Command The command to be executed (join/quit)
	* @param ControllerId The Controller Id on which command should be executed
	* @param StreamingPort The port to stream this player from (for join)
	* @param StreamingIP The IP to stream this game from (for join)
	* @param GameSessionId The id of this game session (for join)
	*/
	virtual bool CCloudyPlayerManagerModule::ExecuteCommand(FString Command, 
		int32 ControllerId, int32 StreamingPort, FString StreamingIP, int32 GameSessionId);


	/**
	* Method to add player to the game by Controller ID.
	*
	* @param ControllerId The Controller ID of the player to be added
	* @param StreamingPort The port to stream this player from
	* @param StreamingIP The IP to stream this game from
	* @param GameSessionId The ID of this game session
	*/
	bool CCloudyPlayerManagerModule::AddPlayer(int32 ControllerId, int32 StreamingPort,
		FString StreamingIP, int32 GameSessionId);


	/**
	* Method to remove player by Controller ID. Sends a delete game session
	* request to CloudyWeb server
	*
	* @param ControllerId The Controller ID of the player to be deleted
	* @param GameSessionId The game session to be removed
	*/
	bool CCloudyPlayerManagerModule::RemovePlayer(int32 ControllerId, int32 GameSessionId);


	/**
	* Singleton-like access to this module's interface.  This is just for
	* convenience! Beware of calling this during the shutdown phase, though.
	* Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
	static inline CCloudyPlayerManagerModule& Get()
	{
		return FModuleManager::LoadModuleChecked< CCloudyPlayerManagerModule >("CloudyPlayerManager");
	}


	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call
	* Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("CloudyPlayerManager");
	}

	/** Class variables */
	int GameSessionIdMapping[]; // maps controller ID to game session ID
};