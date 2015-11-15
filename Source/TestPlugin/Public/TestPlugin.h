// Some copyright should be here...

#pragma once

DECLARE_LOG_CATEGORY_EXTERN(ModuleLog, Log, All)

class FTestPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Input Handlers*/
	bool FTestPluginModule::ExecuteCommand(FString Command, int32 ControllerId);
	bool FTestPluginModule::InputHandler(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint);

	/** Timer */
	bool FTestPluginModule::Tick(float DeltaTime);

	/** Helper Methods*/
	bool FTestPluginModule::SendToClient(FSocket* Socket, FString Msg);
	bool FTestPluginModule::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);
	FString FTestPluginModule::StringFromBinaryArray(const TArray<uint8>& BinaryArray);
	

	/** Class Variables */
	FSocket* TCPConnection;
	FString InputStr;
	bool HasInputStrChanged;

private:

};