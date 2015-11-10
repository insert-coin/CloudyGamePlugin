// Some copyright should be here...

#include "TestPlugin2PrivatePCH.h"
#include "TestPlugin2Commands.h"

#define LOCTEXT_NAMESPACE "FTestPlugin2Module"

void FTestPlugin2Commands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "TestPlugin2", "Execute TestPlugin2 action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
