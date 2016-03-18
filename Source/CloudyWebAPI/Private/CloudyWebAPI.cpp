#include "CloudyWebAPIPrivatePCH.h"
 
#include "CloudyWebAPI.h"

#include "PlatformFeatures.h"
#include "HttpRequestAdapter.h"
#include "HttpModule.h"
#include "IHttpResponse.h"
#include "AllowWindowsPlatformTypes.h"
#include "ThirdParty/libcurl/include/Windows/curl/curl.h"
#include "HideWindowsPlatformTypes.h"
#include "string"

DEFINE_LOG_CATEGORY(CloudyWebAPILog);

static const FString BaseUrl = "http://127.0.0.1:8000";
static const FString AuthUrl = "/api-token-auth/";
static const FString SaveDataUrl = "/save-data/";
static FString Token;

// Automatically starts when UE4 is started.
// Populates the Token variable with the robot user's token.
void CloudyWebAPIImpl::StartupModule()
{
    UE_LOG(CloudyWebAPILog, Warning, TEXT("CloudyWebAPI started"));
    // Token variable will be populated with the robot user's token.
    AttemptAuthentication(TEXT("joel"), TEXT("1234"));
}

// Automatically starts when UE4 is closed
void CloudyWebAPIImpl::ShutdownModule()
{
    UE_LOG(CloudyWebAPILog, Warning, TEXT("CloudyWebAPI stopped"));
}

/**
* This function is called when the libcurl function "CURLOPT_WRITEFUNCTION" 
* demands it. It reads the response data from the server, which can be put
* into a buffer using the libcurl function "CURLOPT_WRITEDATA".
*
*/
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
* Uses the robot user's username and password to obtain an operator token to use for
* various functions.
*
* @param Username        Username of the robot user.
* @param Password        Password of the robot user.
*
* @return Returns true if the authentication was successful. Else, returns false.
*/
bool CloudyWebAPIImpl::AttemptAuthentication(FString Username, FString Password)
{
    bool RequestSuccess = false;

    FString Url = BaseUrl + AuthUrl; // "http://127.0.0.1:8000/api-token-auth/";
    FString ContentString;

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("username"), Username);
    JsonObject->SetStringField(TEXT("password"), Password);

    TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&ContentString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

    TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetURL(Url);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetContentAsString(ContentString);
    HttpRequest->OnProcessRequestComplete().BindRaw(this, &CloudyWebAPIImpl::OnAuthResponseComplete);
    RequestSuccess = HttpRequest->ProcessRequest();

    UE_LOG(CloudyWebAPILog, Warning, TEXT("URL = %s"), *Url);
    UE_LOG(CloudyWebAPILog, Warning, TEXT("ContentString = %s"), *ContentString);

    return RequestSuccess;
}

/**
* Uploads the save file to CloudyWeb.
*
* @param Filename               Filename of the save game file.
* @param PlayerControllerId     PlayerControllerId of the player who is saving the game.
*
* @return Returns true if the file upload was successful. Else, returns false.
*
*/
bool CloudyWebAPIImpl::UploadFile(FString Filename, int32 PlayerControllerId)
{
    bool RequestSuccess = false;

    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    
    FString Url = BaseUrl + SaveDataUrl;
    std::string UrlCString(TCHAR_TO_UTF8(*Url));
    
    // Filepath of .sav file
    FString Filepath = FPaths::GameDir();
    Filepath += "Saved/SaveGames/" + Filename + ".sav";
    std::string filePath(TCHAR_TO_UTF8(*Filepath));
    
    // Get game name
    FString GameName = FApp::GetGameName();
    std::string gameName(TCHAR_TO_UTF8(*GameName));
    
    // Convert PlayerControllerId
    FString playerControllerIdFString = FString::FromInt(PlayerControllerId);
    std::string playerControllerId(TCHAR_TO_UTF8(*playerControllerIdFString));
    
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;
    //static const char buf[] = "Expect:";
    FString AuthHeader = "Authorization: Token " + Token;
    std::string AuthHeaderCString(TCHAR_TO_UTF8(*AuthHeader));
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    /* Fill in the file upload field */
    curl_formadd(&formpost, &lastptr,
        CURLFORM_COPYNAME, "saved_file",
        CURLFORM_FILE, filePath.c_str(),
        CURLFORM_END);
    
    /* Fill in the player controller ID field */
    curl_formadd(&formpost, &lastptr,
        CURLFORM_COPYNAME, "controller",
        CURLFORM_COPYCONTENTS, playerControllerId.c_str(),
        CURLFORM_END);
    
    /* Fill in the game name field */
    curl_formadd(&formpost, &lastptr,
        CURLFORM_COPYNAME, "game",
        CURLFORM_COPYCONTENTS, gameName.c_str(),
        CURLFORM_END);
    
    curl = curl_easy_init();
    /* initialize custom header list (stating that Expect: 100-continue is not
    wanted */
    headerlist = curl_slist_append(headerlist, AuthHeaderCString.c_str());
    if (curl) {
        /* what URL that receives this POST */
        curl_easy_setopt(curl, CURLOPT_URL, UrlCString.c_str());

        /* only disable 100-continue header if explicitly requested */
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

        /* What form to send */
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    
        /* Set up string to write response into */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check if the request was successful */
        if (res == CURLE_OK)
        {
            UE_LOG(CloudyWebAPILog, Warning, TEXT("File upload success!"));
            RequestSuccess = true;
        }
    
        /* always cleanup */
        curl_easy_cleanup(curl);
    
        /* then cleanup the formpost chain */
        curl_formfree(formpost);
        /* free slist */
        curl_slist_free_all(headerlist);
    
        UE_LOG(CloudyWebAPILog, Warning, TEXT("Response data: %s"), UTF8_TO_TCHAR(readBuffer.c_str()));

    }

    return RequestSuccess;
}

/**
* This is a callback function that is called when the "AttemptAuthentication" function
* has completed its request.
*
* @param Request            
* @param Response           Contains the data of the response, including the response code.
* @param bWasSuccessful     Contains true if the request was successful. Else it contains false.
*
*/
void CloudyWebAPIImpl::OnAuthResponseComplete(FHttpRequestPtr Request, 
                                              FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(CloudyWebAPILog, Warning, TEXT("Response Code = %d"), Response->GetResponseCode());

        if (!Response.IsValid())
        {
            UE_LOG(CloudyWebAPILog, Warning, TEXT("Request failed!"));
        }
        else if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
        {
            TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
            TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());
            FJsonSerializer::Deserialize(JsonReader, JsonObject);

            Token = JsonObject->GetStringField("token");
            UE_LOG(CloudyWebAPILog, Warning, TEXT("Token = %s"), *Token);
        }
    }
    else 
    {
        UE_LOG(CloudyWebAPILog, Warning, TEXT("Request failed! Is the server up?"));
    }
}

 
IMPLEMENT_MODULE(CloudyWebAPIImpl, CloudyWebAPI)