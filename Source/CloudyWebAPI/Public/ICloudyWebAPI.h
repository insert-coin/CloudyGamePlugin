#pragma once
 
#include "ModuleManager.h"
#include "Engine.h"
 
/**
* The public interface to this module.  In most cases, this interface is only public to sibling modules
* within this plugin.
*/
class ICloudyWebAPI : public IModuleInterface
{
 
public:
 
	/**
	* Singleton-like access to this module's interface.  This is just for convenience!
	* Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
    static inline ICloudyWebAPI& Get()
	{
        return FModuleManager::LoadModuleChecked< ICloudyWebAPI >("CloudyWebAPI");
	}
 
	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("CloudyWebAPI");
	}

    /**
    * Uploads the save file to CloudyWeb. Used by CloudySaveManager.
    *
    * @param Filename               Filename of the save game file.
    * @param PlayerControllerId     PlayerControllerId of the player who is saving the game.
    *
    * @return Returns true if the file upload was successful. Else, returns false.
    */
    virtual bool UploadFile(FString Filename, int32 PlayerControllerId) = 0;


    /**
    * Downloads the save file from CloudyWeb. Used by CloudySaveManager
    *
    * @param Filename               Filename of the save game file.
    * @param PlayerControllerId     Player controller ID of the player who requested the file.
    *
    * @return Returns true if the file download was successful. Else, returns false.
    *
    */
    virtual bool DownloadFile(FString Filename, int32 PlayerControllerId) = 0;

	/**
	* Makes a request to CloudyWeb server
	*
	* @param ResourceUrl The url of the resource to get, for example /game-session/
	* @param RequestMethod The type of HTTP request, for example GET or DELETE
	*
	* @return Whether the request was successful or not
	*
	*/
	virtual bool MakeRequest(FString ResourceUrl, FString RequestMethod) = 0;

	/**
	* Accessor for HTTP response. Caller may have to wait for valid response.
	*
	* @return The response from the HTTP request from MakeRequest
	*
	*/
	virtual FString GetResponse() = 0;

	/**
	* Accessor for GameId.
	*
	* @return GameId, or -1 if not yet obtained from server
	*
	*/
	virtual int32 GetGameId() = 0;

	/**
	* Accessor for Username, by ControllerId
	*
	* @param ControllerId The controller ID of the user
	* @return Username, or empty string if not a valid user (using ControllerId)
	*
	*/
	virtual FString GetUsername(int32 ControllerId) = 0;
};