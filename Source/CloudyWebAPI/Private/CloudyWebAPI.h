#pragma once
#include "Engine.h"
#include "Http.h"
#include "Networking.h"

DECLARE_LOG_CATEGORY_EXTERN(CloudyWebAPILog, Log, All)
 
class CloudyWebAPIImpl : public ICloudyWebAPI
{
public:
    // These public functions are accessible outside this plugin module:
    bool UploadFile(FString Filename, int32 PlayerControllerId);
    bool DownloadFile(FString Filename, int32 PlayerControllerId);
	bool MakeRequest(FString ResourceUrl, FString RequestMethod);
	FString GetResponse();
	int32 GetGameId();
	FString GetUsername(int32 ControllerId);

private:
    /** IModuleInterface implementation */
    void StartupModule();
    void ShutdownModule();

	/** Authentication and Request API */
    bool AttemptAuthentication();
    void OnAuthResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnGetResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    /** Save and load game */
    void ReadAndStoreSaveFileURL(FString JsonString, int32 PlayerControllerId);
    void GetSaveFileUrl(int32 GameId, FString Username, int32 PlayerControllerId);

	/** TCP Listener and command handling methods */
	bool InputHandler(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint);
	bool CheckConnection(float DeltaTime);
	bool SendToClient(FSocket* Socket, FString Msg);
	FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);
	bool GetCloudyWebData(FString InputStr);
	bool ExecuteCommand(FString Command, int32 ControllerId);


	/** Class Variables */

	FSocket* ListenSocket;
	FSocket* TCPConnection;
	FTcpListener* TcpListener;
	FString InputStr;
	bool HasInputStrChanged;

	/** Game and user variables received from CloudyWeb */
	int32 ControllerId;
	int32 StreamingPort;
	FString StreamingIP;
	int32 GameId;
	TArray<FString> UsernameList;
	int32 GameSessionId;
	FString Command;

};
