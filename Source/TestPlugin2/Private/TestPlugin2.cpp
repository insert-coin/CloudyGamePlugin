// Some copyright should be here...

#include "TestPlugin2PrivatePCH.h"

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "TestPlugin2Style.h"
#include "TestPlugin2Commands.h"

#include "LevelEditor.h"

static const FName TestPlugin2TabName("TestPlugin2");

#define LOCTEXT_NAMESPACE "FTestPlugin2Module"
#define SERVER_NAME "Listener"
#define IP "127.0.0.1"
#define PORT_NO 55555
#define BUFFER_SIZE 1024
#define JOIN_GAME "join"
#define QUIT_GAME "quit"

void FTestPlugin2Module::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FTestPlugin2Style::Initialize();
	FTestPlugin2Style::ReloadTextures();

	FTestPlugin2Commands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTestPlugin2Commands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FTestPlugin2Module::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FTestPlugin2Module::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FTestPlugin2Module::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	
}

void FTestPlugin2Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	
	
	FTestPlugin2Style::Shutdown();

	FTestPlugin2Commands::Unregister();
}

void FTestPlugin2Module::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FTestPlugin2Module::PluginButtonClicked()")),
							FText::FromString(TEXT("TestPlugin2.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	if (GEngine) {
		FText DialogText = FText::Format(
			LOCTEXT("PluginButtonDialogText", "Engine is running"),
			FText::FromString(TEXT("FTestPlugin2Module::PluginButtonClicked()")),
			FText::FromString(TEXT("TestPlugin2.cpp"))
			);
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	}

	CloudyGameTCPServer* ListenerServer = new CloudyGameTCPServer(SERVER_NAME, IP, PORT_NO, BUFFER_SIZE);
	FSocket* ListenerSocket = ListenerServer->ListenerSocket;
	FString Msg = ListenerServer->ReceiveMsg(ListenerSocket);
	UE_LOG(ServerLog, Warning, TEXT("Received by server: %s"), *Msg);
	HandleInput(Msg);
	/*
	if (msg == "Join game") {
		UGameInstance* gameInstance = GEngine->GameViewport->GetGameInstance();
		int ControllerId = 1;
		FString Error;
		gameInstance->CreateLocalPlayer(ControllerId, Error, true);
	}
	*/
}

void FTestPlugin2Module::HandleInput(FString input) {
	UE_LOG(ServerLog, Warning, TEXT("String is %s"), *input);
	FString Command, ControllerIdStr;
	input.Split(" ", &Command, &ControllerIdStr, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	int32 ControllerId = FCString::Atoi(*ControllerIdStr);
	UE_LOG(ServerLog, Warning, TEXT("Command: %sxx ControllerId: %d"), *Command, ControllerId);
	UGameInstance* gameInstance = GEngine->GameViewport->GetGameInstance();

	if (Command == JOIN_GAME) {
		FString Error;
		gameInstance->CreateLocalPlayer(ControllerId, Error, true);

	}
	else if (Command == QUIT_GAME) {
		ULocalPlayer* const ExistingPlayer = gameInstance->FindLocalPlayerFromControllerId(ControllerId);
		if (ExistingPlayer != NULL)
		{
			gameInstance->RemoveLocalPlayer(ExistingPlayer);
		}
	}
	

}

void FTestPlugin2Module::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FTestPlugin2Commands::Get().PluginAction);
}

void FTestPlugin2Module::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FTestPlugin2Commands::Get().PluginAction);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTestPlugin2Module, TestPlugin2)