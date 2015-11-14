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
	void FTestPluginModule::ExecuteCommand(FString Command, int32 ControllerId);
	bool FTestPluginModule::InputHandler(FSocket* ConnectionSocket, const FIPv4Endpoint& Endpoint);

	/** Helper Methods*/
	bool FTestPluginModule::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);
	FString FTestPluginModule::StringFromBinaryArray(const TArray<uint8>& BinaryArray);

	// timer
	bool FTestPluginModule::Tick(float DeltaTime);

	FString InputStr;
	bool HasInputStrChanged;

private:

};