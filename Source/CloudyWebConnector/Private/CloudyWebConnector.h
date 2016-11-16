#pragma once
#include "Engine.h"
#include "Http.h"
#include "Networking.h"

DECLARE_LOG_CATEGORY_EXTERN(CloudyWebConnectorLog, Log, All)
 
class CloudyWebConnectorImpl : public ICloudyWebConnector
{
private:
    /** IModuleInterface implementation */
    void StartupModule();
    void ShutdownModule();

    /** TCP Listener and command handling methods */
    bool InputHandler(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint);
    bool CheckConnection(float DeltaTime);
    FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);
    bool GetCloudyWebData(FString InputStr);

    /** Class Variables */

    FSocket* ListenSocket;
    FSocket* TCPConnection;
    FTcpListener* TcpListener;
    FString InputStr;
    bool HasInputStrChanged;

    /** Game and user variables received from CloudyWeb */
    int32 ControllerId;
    FString Command;

};
