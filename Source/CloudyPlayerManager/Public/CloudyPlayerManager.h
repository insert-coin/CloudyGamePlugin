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
	*/
	virtual bool ExecuteCommand(FString Command, int32 ControllerId);


	/**
	* Method to add player to the game by Controller ID.
	*
	* @param ControllerId The Controller ID of the player to be added
	*/
	bool AddPlayer(int32 ControllerId);


	/**
	* Method to remove player by Controller ID. Sends a delete game session
	* request to CloudyWeb server
	*
	* @param ControllerId The Controller ID of the player to be deleted
	*/
	bool RemovePlayer(int32 ControllerId);


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
};