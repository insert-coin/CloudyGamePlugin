# CloudyPanelPlugin
## Description

Plugin to interface between CloudyPanel and Engine

## Setup

In your game folder, create a folder named 'Plugins' if it doesn't exist. Put CloudyPanelPlugin in your Plugins folder. Build and run your game. The plugin should show up in Menu > Edit > Plugins. 
To run split screen correctly, you need to modify Unreal Engine. Go to GameViewportClient.cpp and edit the function UGameViewportClient::LayoutPlayers(). Change SplitType to 4 player. Edit the code as follows:

	Comment out this line: const ESplitScreenType::Type SplitType = GetCurrentSplitscreenConfiguration();
	Add this line: const ESplitScreenType::Type SplitType = ESplitScreenType::FourPlayer;
	
Set the IP in the function CCloudyPanelPluginModule::SetUpPlayer before streaming.

## Usage
Split screen is now working. To join game, you can test using OtherFiles/sendTCP.py. Run the file and input 00000001 to add Player 1. More details in the file. Plugin streams to HTTP, so use VLC to catch the frames. The ports are:

ControllerId 1: <your HTTP IP>:30000,

ControllerId 2: <your HTTP IP>:30001

ControllerId 3: <your HTTP IP>:30002

ControllerId 4: <your HTTP IP>:30003


Note, all files from ffmpeg (output video, sdp file, log file out.txt etc) are probably generated in your Unreal Engine\Engine\Binaries\Win64 folder.


# CloudySaveManager
### Setup
- In `YourProject/Source/YourProject/YourProject.Build.cs`:
  - Ensure that `CloudySaveManager` is added to your `PrivateDependencyModuleNames`. 
  - E.g. `PrivateDependencyModuleNames.AddRange(new string[] { "CloudySaveManager" });`

- In your .cpp file where you want to use our custom `Cloudy_SaveGameToSlot` functions: 
  - Ensure that `#include "ICloudySaveManager.h"` is included.

## Usage
`Cloudy_SaveGameToSlot` takes in the same three functions as Unreal Engine's `SaveGameToSlot`, with an additional fourth parameter: the player controller index.

API:
```cpp
UFUNCTION(BlueprintCallable, Category="Game")
virtual bool Cloudy_SaveGameToSlot
(
    USaveGame * SaveGameObject,
    const FString & SlotName,
    const int32 UserIndex,
    const int32 PCID // Player Controller ID of the player you are saving
)
```

Example: 
```cpp
#include "ICloudySaveManager.h"

// Create a save game object
UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
// Save the game
ICloudySaveManager::Get().Cloudy_SaveGameToSlot(SaveGameInstance, "SaveGame1", SaveGameInstance->UserIndex, 0);
```
