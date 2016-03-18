#pragma once
 
#include "ModuleManager.h"
#include "GameFramework/SaveGame.h"
#include "Engine.h"
 
/**
* The public interface to this module.  In most cases, this interface is only public to sibling modules
* within this plugin.
*/
class ICloudySaveManager : public IModuleInterface
{
 
public:
 
	/**
	* Singleton-like access to this module's interface.  This is just for convenience!
	* Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
    static inline ICloudySaveManager& Get()
	{
        return FModuleManager::LoadModuleChecked< ICloudySaveManager >("CloudySaveManager");
	}
 
	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("CloudySaveManager");
	}
 
    // REMEMBER TO PUT = 0 AT THE END
    virtual bool Cloudy_SaveGameToSlot(USaveGame* SaveGameObject, const FString& SlotName, const int32 UserIndex, const int32 PCID) = 0;

    virtual USaveGame* Cloudy_LoadGameFromSlot(const FString& SlotName, const int32 UserIndex) = 0;
};