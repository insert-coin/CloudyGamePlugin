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
};