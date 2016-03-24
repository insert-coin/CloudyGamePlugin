#pragma once
#include "Engine.h"
#include "Http.h"

DECLARE_LOG_CATEGORY_EXTERN(CloudyWebAPILog, Log, All)
 
class CloudyWebAPIImpl : public ICloudyWebAPI
{
public:
    // These public functions are accessible outside this plugin module:
    bool UploadFile(FString Filename, int32 PlayerControllerId);
    bool DownloadFile(FString Filename, int32 PlayerControllerId);
	bool MakeRequest(FString ResourceUrl, FString RequestMethod);
	FString GetResponse();

private:
    /** IModuleInterface implementation */
    void StartupModule();
    void ShutdownModule();

    bool AttemptAuthentication();
    void OnAuthResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void ReadAndStoreSaveFileURL(FString JsonString, int32 PlayerControllerId);
	void OnGetResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
