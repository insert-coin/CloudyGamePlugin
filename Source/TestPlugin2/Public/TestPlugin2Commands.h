// Some copyright should be here...

#pragma once

#include "SlateBasics.h"
#include "TestPlugin2Style.h"

class FTestPlugin2Commands : public TCommands<FTestPlugin2Commands>
{
public:

	FTestPlugin2Commands()
		: TCommands<FTestPlugin2Commands>(TEXT("TestPlugin2"), NSLOCTEXT("Contexts", "TestPlugin2", "TestPlugin2 Plugin"), NAME_None, FTestPlugin2Style::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
