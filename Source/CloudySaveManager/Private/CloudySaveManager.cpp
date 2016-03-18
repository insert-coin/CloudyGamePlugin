#include "CloudySaveManagerPrivatePCH.h"
 
#include "CloudySaveManager.h"
#include "../../CloudyWebAPI/Public/ICloudyWebAPI.h"

#include "PlatformFeatures.h"
#include "GameFramework/SaveGame.h"

DEFINE_LOG_CATEGORY(CloudySaveManagerLog)

static const int UE4_SAVEGAME_FILE_TYPE_TAG = 0x53415647;		// "sAvG"
static const int UE4_SAVEGAME_FILE_VERSION = 1;

// Automatically starts and shuts down when UE4 is started/closed
void CloudySaveManagerImpl::StartupModule()
{
    UE_LOG(CloudySaveManagerLog, Warning, TEXT("CloudySaveManager started"));
}
 
void CloudySaveManagerImpl::ShutdownModule()
{
    UE_LOG(CloudySaveManagerLog, Warning, TEXT("CloudySaveManager stopped"));
}

bool CloudySaveManagerImpl::Cloudy_SaveGameToSlot(USaveGame* SaveGameObject, const FString& SlotName, 
                                                  const int32 UserIndex, const int32 PCID)
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
        SaveSystem->SaveGame(false, *SlotName, UserIndex, ObjectBytes);

        bSuccess = ICloudyWebAPI::Get().UploadFile(SlotName, PCID);
    }

    return bSuccess;
}

USaveGame* CloudySaveManagerImpl::Cloudy_LoadGameFromSlot(const FString& SlotName, 
                                                          const int32 UserIndex)
{
    // Load from CloudyWeb
    // ...
    
    USaveGame* OutSaveGameObject = NULL;
    
    ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
    // If we have a save system and a valid name..
    if (SaveSystem && (SlotName.Len() > 0))
    {
        // Load raw data from slot
        TArray<uint8> ObjectBytes;
        bool bSuccess = SaveSystem->LoadGame(false, *SlotName, UserIndex, ObjectBytes);
        if (bSuccess)
        {
            FMemoryReader MemoryReader(ObjectBytes, true);

            int32 FileTypeTag;
            MemoryReader << FileTypeTag;

            int32 SavegameFileVersion;
            if (FileTypeTag != UE4_SAVEGAME_FILE_TYPE_TAG)
            {
                // this is an old saved game, back up the file pointer to the beginning and assume version 1
                MemoryReader.Seek(0);
                SavegameFileVersion = 1;

                // Note for 4.8 and beyond: if you get a crash loading a pre-4.8 version of your savegame file and 
                // you don't want to delete it, try uncommenting these lines and changing them to use the version 
                // information from your previous build. Then load and resave your savegame file.
                //MemoryReader.SetUE4Ver(MyPreviousUE4Version);				// @see GPackageFileUE4Version
                //MemoryReader.SetEngineVer(MyPreviousEngineVersion);		// @see FEngineVersion::Current()
            }
            else
            {
                // Read version for this file format
                MemoryReader << SavegameFileVersion;

                // Read engine and UE4 version information
                int32 SavedUE4Version;
                MemoryReader << SavedUE4Version;

                FEngineVersion SavedEngineVersion;
                MemoryReader << SavedEngineVersion;

                MemoryReader.SetUE4Ver(SavedUE4Version);
                MemoryReader.SetEngineVer(SavedEngineVersion);
            }

            // Get the class name
            FString SaveGameClassName;
            MemoryReader << SaveGameClassName;

            // Try and find it, and failing that, load it
            UClass* SaveGameClass = FindObject<UClass>(ANY_PACKAGE, *SaveGameClassName);
            if (SaveGameClass == NULL)
            {
                SaveGameClass = LoadObject<UClass>(NULL, *SaveGameClassName);
            }

            // If we have a class, try and load it.
            if (SaveGameClass != NULL)
            {
                OutSaveGameObject = NewObject<USaveGame>(GetTransientPackage(), SaveGameClass);

                FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
                OutSaveGameObject->Serialize(Ar);
            }
        }
    }
    
    return OutSaveGameObject;
}

 
IMPLEMENT_MODULE(CloudySaveManagerImpl, CloudySaveManager)