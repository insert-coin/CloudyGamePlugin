#include "CloudySaveManagerPrivatePCH.h"
 
#include "CloudySaveManager.h"
#include "PlatformFeatures.h"
#include "GameFramework/SaveGame.h"

static const int UE4_SAVEGAME_FILE_TYPE_TAG = 0x53415647;		// "sAvG"
static const int UE4_SAVEGAME_FILE_VERSION = 1;

void CloudySaveManagerImpl::StartupModule()
{
}
 
void CloudySaveManagerImpl::ShutdownModule()
{
}
 
bool CloudySaveManagerImpl::Cloudy_SaveGameToSlot(USaveGame* SaveGameObject, const FString& SlotName, const int32 UserIndex, const int32 PCID)//APlayerController const* PC)
{
    FString theName = SlotName;

    bool bSuccess = false;

    ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
    // If we have a system and an object to save and a save name...
    if (SaveSystem && (SaveGameObject != NULL) && (SlotName.Len() > 0))
    {
        TArray<uint8> ObjectBytes;
        FMemoryWriter MemoryWriter(ObjectBytes, true);

        // write file type tag. identifies this file type and indicates it's using proper versioning
        // since older UE4 versions did not version this data.
        int32 FileTypeTag = UE4_SAVEGAME_FILE_TYPE_TAG;
        MemoryWriter << FileTypeTag;

        // Write version for this file format
        int32 SavegameFileVersion = UE4_SAVEGAME_FILE_VERSION;
        MemoryWriter << SavegameFileVersion;

        // Write out engine and UE4 version information
        MemoryWriter << GPackageFileUE4Version;
        FEngineVersion SavedEngineVersion = GEngineVersion;
        MemoryWriter << SavedEngineVersion;

        // Write the class name so we know what class to load to
        FString SaveGameClassName = SaveGameObject->GetClass()->GetName();
        MemoryWriter << SaveGameClassName;
        
        // Then save the object state, replacing object refs and names with strings
        FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
        SaveGameObject->Serialize(Ar);

        // Stuff that data into the save system with the desired file name
        bSuccess = SaveSystem->SaveGame(false, *SlotName, UserIndex, ObjectBytes);

        // Get player ID, upload to CloudyWeb
        //ULocalPlayer::GetControllerId();
        //ULocalPlayer * LP = Cast<ULocalPlayer>(PC->Player);
        //int ControllerID = LP->GetControllerId();

        //UE_LOG(LogTemp, Warning, TEXT("CID = %d"), ControllerID);
    }

    return bSuccess;
}

 
IMPLEMENT_MODULE(CloudySaveManagerImpl, CloudySaveManager)