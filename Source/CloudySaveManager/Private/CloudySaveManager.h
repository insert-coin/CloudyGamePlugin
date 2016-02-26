#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
//#include "GameplayStatics.generated.h"
#include "Engine.h"
#include "GameFramework/SaveGame.h"

//
// Forward declarations.
//
//class USaveGame;
 
class CloudySaveManagerImpl : public ICloudySaveManager
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
 
    /**
    *	Save the contents of the SaveGameObject to a slot, and upload it to CloudyGame's cloud.
    *	@param SaveGameObject	Object that contains data about the save game that we want to write out
    *	@param SlotName			Name of save game slot to save to.
    *   @param UserIndex		For some platforms, master user index to identify the user doing the saving.
    *   @param PC               PlayerController object of the player to save
    *	@return					Whether we successfully saved this information
    */
    UFUNCTION(BlueprintCallable, Category = "Game")
        bool Cloudy_SaveGameToSlot(USaveGame* SaveGameObject, const FString& SlotName, const int32 UserIndex, const int32 PCID);// APlayerController const* PC);

};