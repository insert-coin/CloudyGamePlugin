#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
//#include "GameplayStatics.generated.h"
#include "Engine.h"
#include "GameFramework/SaveGame.h"
#include "Http.h"

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
    *   @param PCID             PlayerController object of the player to save
    *	@return					Whether we successfully saved this information
    */
    UFUNCTION(BlueprintCallable, Category = "CloudyGame")
        bool Cloudy_SaveGameToSlot(USaveGame* SaveGameObject, const FString& SlotName, const int32 UserIndex, const int32 PCID, bool IsAutosaved); // APlayerController const* PC);

    bool AttemptAuthentication(FString Username, FString Password);
    bool UploadFile(FString Filename, int32 PlayerControllerId, bool isAutosaved);
    void OnResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnAuthResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    /**
    *	Save the contents of the SaveGameObject to a slot.
    *	@param SlotName			Name of save game slot to save to.
    *   @param UserIndex		For some platforms, master user index to identify the user doing the saving.
    *	@return SaveGameObject	Object containing loaded game state (NULL if load fails)
    */
    UFUNCTION(BlueprintCallable, Category = "CloudyGame")
        USaveGame* Cloudy_LoadGameFromSlot(const FString& SlotName, const int32 UserIndex, bool IsAutosaved);
};