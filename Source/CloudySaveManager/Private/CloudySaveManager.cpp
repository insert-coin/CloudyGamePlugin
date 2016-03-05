#include "CloudySaveManagerPrivatePCH.h"
 
#include "CloudySaveManager.h"
#include "PlatformFeatures.h"
#include "GameFramework/SaveGame.h"
#include "HttpRequestAdapter.h"
#include "HttpModule.h"
#include "IHttpResponse.h"
#include "Base64.h"

static const int UE4_SAVEGAME_FILE_TYPE_TAG = 0x53415647;		// "sAvG"
static const int UE4_SAVEGAME_FILE_VERSION = 1;
static const FString BaseUrl = "http://127.0.0.1:8000";
static const FString AuthUrl = "/api-token-auth/";
static const FString SaveDataUrl = "/save-data/";
static FString Token;

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
        SaveSystem->SaveGame(false, *SlotName, UserIndex, ObjectBytes);

        // Get player ID, upload to CloudyWeb
        //ULocalPlayer::GetControllerId();
        //ULocalPlayer * LP = Cast<ULocalPlayer>(PC->Player);
        //int ControllerID = LP->GetControllerId();

        //UE_LOG(LogTemp, Warning, TEXT("CID = %d"), ControllerID);

        //bSuccess = AttemptAuthentication(TEXT("user1"), TEXT("1234"));
        bSuccess = UploadFile(SlotName);
    }

    return bSuccess;
}

bool CloudySaveManagerImpl::AttemptAuthentication(FString username, FString password)
{
    bool RequestSuccess = false;

    FString Url = BaseUrl + AuthUrl; // "http://127.0.0.1:8000/api-token-auth/";
    Url = "http://posttestserver.com/post.php?dir=bloodelves88";
    FString ContentString;

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("username"), username);
    JsonObject->SetStringField(TEXT("password"), password);

    TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&ContentString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

    TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetURL(Url);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetContentAsString(ContentString);
    HttpRequest->OnProcessRequestComplete().BindRaw(this, &CloudySaveManagerImpl::OnAuthResponseComplete);
    RequestSuccess = HttpRequest->ProcessRequest();

    UE_LOG(LogTemp, Warning, TEXT("URL = %s"), *Url);
    UE_LOG(LogTemp, Warning, TEXT("ContentString = %s"), *ContentString);

    return RequestSuccess;
}

bool CloudySaveManagerImpl::UploadFile(FString filename)
{
    bool RequestSuccess = false;

    FString Url = BaseUrl + SaveDataUrl; // "http://127.0.0.1:8000/save-data/";
    Url = "http://posttestserver.com/post.php?dir=bloodelves88";
    FString ContentString;

    // Filepath of .sav file
    FString Filepath = FPaths::GameDir();
    Filepath += "Saved/SaveGames/" + filename + ".sav";
    UE_LOG(LogTemp, Warning, TEXT("Filepath = %s"), *Filepath);

    // Load .sav file into array
    TArray<uint8> FileRawData;
    FFileHelper::LoadFileToArray(FileRawData, *Filepath);

    // prepare json data
    FString JsonString;
    TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);
    
    JsonWriter->WriteObjectStart();
    JsonWriter->WriteValue("saved_file", filename);
    JsonWriter->WriteValue("file_object", FBase64::Encode(FileRawData));
    JsonWriter->WriteValue("is_autosaved", false);
    JsonWriter->WriteValue("game", "wow");
    JsonWriter->WriteValue("user", "1");
    JsonWriter->WriteObjectEnd();
    JsonWriter->Close();

    // the json request
    TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindRaw(this, &CloudySaveManagerImpl::OnResponseComplete);
    //HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=--d79163f5f5y42f81edd3--"));
    //HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    //HttpRequest->SetHeader(TEXT("Content-Disposition"), TEXT("form-data; name=\"submitted\"; filename=\"SaveGame1.sav\""));
    HttpRequest->SetURL(Url);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetContentAsString(JsonString);
    //HttpRequest->SetContent(FileRawData);
    RequestSuccess = HttpRequest->ProcessRequest();

    return RequestSuccess;
}

void CloudySaveManagerImpl::OnResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        FString MessageBody;

        UE_LOG(LogTemp, Warning, TEXT("Response Code = %d"), Response->GetResponseCode());

        if (!Response.IsValid())
        {
            MessageBody = "{\"success\":\"Error: Unable to process HTTP Request!\"}";
            GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, TEXT("Request failed!"));

            UE_LOG(LogTemp, Warning, TEXT("Request failed!"));
        }
        else if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
        {
            //MessageBody = Response->GetContentAsString();
            //GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, TEXT("Request success!"));

            TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
            TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());
            FJsonSerializer::Deserialize(JsonReader, JsonObject);

            //MessageBody = JsonObject->GetStringField("token");

            //UE_LOG(LogTemp, Warning, TEXT("Token = %s"), *MessageBody);
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, TEXT("Request error!"));
            MessageBody = FString::Printf(TEXT("{\"success\":\"HTTP Error: %d\"}"), Response->GetResponseCode());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Request failed! Is the server up?"));
    }
}

void CloudySaveManagerImpl::OnAuthResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        FString MessageBody;

        UE_LOG(LogTemp, Warning, TEXT("Response Code = %d"), Response->GetResponseCode());

        if (!Response.IsValid())
        {
            MessageBody = "{\"success\":\"Error: Unable to process HTTP Request!\"}";
            GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, TEXT("Request failed!"));

            UE_LOG(LogTemp, Warning, TEXT("Request failed!"));
        }
        else if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
        {
            //MessageBody = Response->GetContentAsString();
            //GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, TEXT("Request success!"));

            TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
            TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());
            FJsonSerializer::Deserialize(JsonReader, JsonObject);

            Token = JsonObject->GetStringField("token");

            UE_LOG(LogTemp, Warning, TEXT("Token = %s"), *Token);
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, TEXT("Request error!"));
            MessageBody = FString::Printf(TEXT("{\"success\":\"HTTP Error: %d\"}"), Response->GetResponseCode());
        }
    }
    else 
    {
        UE_LOG(LogTemp, Warning, TEXT("Request failed! Is the server up?"));
    }
}

USaveGame* CloudySaveManagerImpl::Cloudy_LoadGameFromSlot(const FString& SlotName, const int32 UserIndex)
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