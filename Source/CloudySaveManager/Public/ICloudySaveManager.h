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

    /**
    *   Save the contents of the SaveGameObject to a slot, and upload it to CloudyGame's cloud.
    *   @param SaveGameObject   Object that contains data about the save game that we want to write out
    *   @param SlotName         Name of save game slot to save to.
    *   @param UserIndex        For some platforms, master user index to identify the user doing the saving.
    *   @param PCID             PlayerController number of the player to save game for
    *
    *   @return                 Whether we successfully saved this information
    */
    UFUNCTION(BlueprintCallable, Category = "CloudyGame")
    virtual bool Cloudy_SaveGameToSlot(USaveGame* SaveGameObject, const FString& SlotName, 
                                       const int32 UserIndex, const int32 PCID) = 0;

    /**
    *   Download the save file from CloudyGame's cloud, then save the contents of the SaveGameObject to a slot.
    *   @param SlotName             Name of save game slot to save to.
    *   @param UserIndex            For some platforms, master user index to identify the user doing the saving.
    *   @param PCID                 PlayerController number of the player to to load game for
    *
    *   @return SaveGameObject      Object containing loaded game state (NULL if load fails)
    */
    UFUNCTION(BlueprintCallable, Category = "CloudyGame")
    virtual USaveGame* Cloudy_LoadGameFromSlot(const FString& SlotName, const int32 UserIndex, const int32 PCID) = 0;
};