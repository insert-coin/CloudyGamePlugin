#include "CloudySaveManagerPrivatePCH.h"
 
#include "CloudySaveManager.h"
#include "PlatformFeatures.h"
#include "GameFramework/SaveGame.h"
#include "HttpRequestAdapter.h"
#include "HttpModule.h"
#include "IHttpResponse.h"

static const int UE4_SAVEGAME_FILE_TYPE_TAG = 0x53415647;		// "sAvG"
static const int UE4_SAVEGAME_FILE_VERSION = 1;
static const FString BaseUrl = "http://127.0.0.1:8000";
static const FString AuthUrl = "/api-token-auth/";

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

        bSuccess = AttemptAuthentication(TEXT("user1"), TEXT("1234"));
    }

    return bSuccess;
}

bool CloudySaveManagerImpl::AttemptAuthentication(FString username, FString password)
{
    bool RequestSuccess = false;

    FString Url = BaseUrl + AuthUrl; // "http://127.0.0.1:8000/api-token-auth/";
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
    HttpRequest->OnProcessRequestComplete().BindRaw(this, &CloudySaveManagerImpl::OnResponseComplete);
    RequestSuccess = HttpRequest->ProcessRequest();

    UE_LOG(LogTemp, Warning, TEXT("URL = %s"), *Url);
    UE_LOG(LogTemp, Warning, TEXT("ContentString = %s"), *ContentString);

    return RequestSuccess;
}

void CloudySaveManagerImpl::OnResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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

        MessageBody = JsonObject->GetStringField("token");
        
        UE_LOG(LogTemp, Warning, TEXT("Token = %s"), *MessageBody);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, TEXT("Request error!"));
        MessageBody = FString::Printf(TEXT("{\"success\":\"HTTP Error: %d\"}"), Response->GetResponseCode());
    }

}
 
IMPLEMENT_MODULE(CloudySaveManagerImpl, CloudySaveManager)