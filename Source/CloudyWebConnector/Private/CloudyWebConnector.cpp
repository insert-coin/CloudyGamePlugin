#include "CloudyWebConnectorPrivatePCH.h"
 
#include "CloudyWebConnector.h"

#include "PlatformFeatures.h"
#include "HttpRequestAdapter.h"
#include "HttpModule.h"
#include "IHttpResponse.h"
#include "string"
#include "../../CloudyPlayerManager/Public/CloudyPlayerManager.h"

DEFINE_LOG_CATEGORY(CloudyWebConnectorLog);

#define SERVER_NAME "Listener"
#define SERVER_ENDPOINT FIPv4Endpoint(FIPv4Address(0, 0, 0, 0), 55556)
#define CONNECTION_THREAD_TIME 5 // in seconds
#define BUFFER_SIZE 1024

// Automatically starts when UE4 is started.
// Populates the Token variable with the robot user's token.
void CloudyWebConnectorImpl::StartupModule()
{
    UE_LOG(CloudyWebConnectorLog, Warning, TEXT("CloudyWebConnector started"));

    // Set up socket listener to receive commands from CloudyWeb

    //Create Socket
    FIPv4Endpoint Endpoint(SERVER_ENDPOINT);
    ListenSocket = FTcpSocketBuilder(SERVER_NAME).AsReusable().BoundToEndpoint(Endpoint).Listening(8);

    //Set Buffer Size
    int32 NewSize = 0;
    ListenSocket->SetReceiveBufferSize(BUFFER_SIZE, NewSize);

    TcpListener = new FTcpListener(*ListenSocket, CONNECTION_THREAD_TIME);
    TcpListener->OnConnectionAccepted().BindRaw(this, &CloudyWebConnectorImpl::InputHandler);

    FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &CloudyWebConnectorImpl::CheckConnection), CONNECTION_THREAD_TIME);


    // initialise class variables
    InputStr = "";
    HasInputStrChanged = false;

}

// Automatically starts when UE4 is closed
void CloudyWebConnectorImpl::ShutdownModule()
{
    UE_LOG(CloudyWebConnectorLog, Warning, TEXT("CloudyWebConnector stopped"));
    delete TcpListener;
    ListenSocket->Close();
}

/**
* Parses string and stores as global variables for access by other modules
*
* @param InputStr Input string to parse
* @return Whether parsing was successful or not
*/
bool CloudyWebConnectorImpl::GetCloudyWebData(FString InputString)
{
    bool isSuccessful = false;
    UE_LOG(CloudyWebConnectorLog, Error, TEXT("Input String = %s"), *InputString);

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    TSharedRef<TJsonReader<TCHAR>>JsonReader = TJsonReaderFactory<TCHAR>::Create(InputString);
    isSuccessful = FJsonSerializer::Deserialize(JsonReader, JsonObject);

    // these 2 fields will be populated for both join/quit commands
    Command = JsonObject->GetStringField("command");
    ControllerId = JsonObject->GetIntegerField("controller");
    
    return isSuccessful;
}


/**
* Timer to periodically check for join/quit signals from client, and call
* appropriate input handler.
*
* @param DeltaTime Time taken by method
*/
bool CloudyWebConnectorImpl::CheckConnection(float DeltaTime)
{
    bool Success = false;
    if (HasInputStrChanged) 
    {
        if (GEngine->GameViewport != nullptr && GIsRunning && IsInGameThread())
        {
            UE_LOG(CloudyWebConnectorLog, Warning, TEXT("Success! input str: %s"), *InputStr);
            GetCloudyWebData(InputStr);
            UE_LOG(CloudyWebConnectorLog, Warning, TEXT("Success! Controllerid: %d command: %s"), ControllerId, *Command);
            Success = CCloudyPlayerManagerModule::Get().ExecuteCommand(Command, ControllerId);
            InputStr = "";
            HasInputStrChanged = false;
        }        
    }

    return true; // continue timer to check for requests
}

/**
* Handles input passed by TCP listener
*
* @param ConnectionSocket The TCP socket connecting the listener and client
* @param Endpoint The endpoint of the socket connection
*/
bool CloudyWebConnectorImpl::InputHandler(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint)
{

    TArray<uint8> ReceivedData;
    uint32 Size;

    // wait for data to arrive
    while (!(ConnectionSocket->HasPendingData(Size)));

    // handle data - change global InputStr
    ReceivedData.SetNumUninitialized(Size);

    int32 Read = 0;
    ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
    FString ReceivedString = StringFromBinaryArray(ReceivedData);

    InputStr = ReceivedString;
    HasInputStrChanged = true;

    TCPConnection = ConnectionSocket;

    return true;

}


//Rama's String From Binary Array
//This function requires #include <string>
FString CloudyWebConnectorImpl::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
    //Create a string from a byte array!
    std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
    return FString(cstr.c_str());
}
 
IMPLEMENT_MODULE(CloudyWebConnectorImpl, CloudyWebConnector)