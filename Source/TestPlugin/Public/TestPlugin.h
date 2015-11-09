// Some copyright should be here...

#pragma once

#include "ModuleManager.h"
#include "Networking.h"

DECLARE_LOG_CATEGORY_EXTERN(ModuleLog, Log, All)

class FTestPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** TCP implementation*/
	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;

	FSocket* FTestPluginModule::CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize);
	bool FTestPluginModule::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);
	FString FTestPluginModule::StringFromBinaryArray(const TArray<uint8>& BinaryArray);
	FString FTestPluginModule::ReceiveMsg(FSocket* ListenerSocket);

};