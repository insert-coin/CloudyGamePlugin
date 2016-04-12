#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine.h"
#include "GameFramework/SaveGame.h"

DECLARE_LOG_CATEGORY_EXTERN(CloudySaveManagerLog, Log, All)
 
class CloudySaveManagerImpl : public ICloudySaveManager
{
public:
    /** IModuleInterface implementation */
    void StartupModule();
    void ShutdownModule();
 
    bool Cloudy_SaveGameToSlot(USaveGame* SaveGameObject, const FString& SlotName, 
                                   const int32 UserIndex, const int32 PCID);

    USaveGame* Cloudy_LoadGameFromSlot(const FString& SlotName, const int32 UserIndex, const int32 PCID);
};
