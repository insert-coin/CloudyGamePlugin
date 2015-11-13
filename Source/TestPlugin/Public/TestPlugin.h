// Some copyright should be here...

#pragma once

#include "Server/CloudyGameTCP.h"
//#include "Server/CloudyGameTCP.cpp"


DECLARE_LOG_CATEGORY_EXTERN(ModuleLog, Log, All)

class FTestPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};