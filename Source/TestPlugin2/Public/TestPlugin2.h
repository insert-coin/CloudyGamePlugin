// Some copyright should be here...

#pragma once

#include "ModuleManager.h"



class FToolBarBuilder;
class FMenuBuilder;

class FTestPlugin2Module : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);
	void HandleInput(FString input);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};