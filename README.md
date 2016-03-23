# CloudyPanelPlugin
## Description

Module to interface between CloudyPanel and Engine, for joining and quitting game.

## Setup

In your game folder, create a folder named 'Plugins' if it doesn't exist. Put CloudyPanelPlugin in your Plugins folder. Build and run your game. The plugin should show up in Menu > Edit > Plugins.

## Usage

This plugin currently supports join game and quit game. To test, send the following codes via TCP to 127.0.0.1:55556 :

00000001 - join game with controller id 1

00000002 - join game with controller id 2

00010001 - quit game with controller id 1

00010002 - quit game with controller id 2

and so on. 

OtherFiles/sendTCP.py has been included to assist testing.

# CloudyStream
** Description

Module for streaming

## Setup

Put ffmpeg executable into your Unreal Engine\Engine\Binaries\Win64 folder.

Modify Unreal Engine. Go to UGameViewportClient.cpp and edit the function UGameViewportClient::LayoutPlayers(). Change SplitType to 4 player. Edit the code as follows:

Comment out this line: const ESplitScreenType::Type SplitType = GetCurrentSplitscreenConfiguration();

Add this line: const ESplitScreenType::Type SplitType = ESplitScreenType::FourPlayer;

## Usage

Open streams (in vlc/ThinClient etc media players) using the following addresses:

Player 0: http://<your public IP>:30000

Player 1: http://<your public IP>:30001

Player 2: http://<your public IP>:30002

Player 3: http://<your public IP>:30003


# CloudySaveManager
## Description

Module to provide customized save game and load game API. This API will allow the game developer to use our custom save/load game functions to upload the player's save game file to our cloud.

## Setup
- Assume that the CloudyPanelPlugin has been successfully installed. If not, please read the setup instructions at the top of this readme.

- In `YourProject/Source/YourProject/YourProject.Build.cs`:
  - Ensure that `CloudySaveManager` is added to your `PrivateDependencyModuleNames`. 
  - E.g. `PrivateDependencyModuleNames.AddRange(new string[] { "CloudySaveManager" });`

- In your .cpp file where you want to use our custom `Cloudy_SaveGameToSlot` functions: 
  - Ensure that `#include "ICloudySaveManager.h"` is included.

## Usage
`Cloudy_SaveGameToSlot` takes in the same three functions as Unreal Engine's `SaveGameToSlot`, with an additional parameter: the player controller index.

API:
```cpp
UFUNCTION(BlueprintCallable, Category="Game")
virtual bool Cloudy_SaveGameToSlot
(
    USaveGame * SaveGameObject,
    const FString & SlotName,
    const int32 UserIndex,
    const int32 PCID, // Player Controller ID of the player you are saving
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

# CloudyWebAPI
## Description

Module to provide network API for communication to the CloudyWeb server.

## Setup
- Assume that the CloudyPanelPlugin has been successfully installed. If not, please read the setup instructions at the top of this readme.

- In the .cpp file where you want to use any public functions in this module: 
  - Ensure that `#include "../../CloudyWebAPI/Public/ICloudyWebAPI.h"` is included.
  
## Usage
Assume that we want to use the `UploadFile` function from this module. We call the function this way:

```cpp
ICloudyWebAPI::Get().UploadFile(Filename, PlayerControllerId);
```

### Adding more API
- `CloudyWebAPI.cpp` contains all the function logic. Do your work here.
- `CloudyWebAPI.h` contains all the function declaration. Declare all your functions here.
- `ICloudyWebAPI.h` contains public function declarations. Only declare functions here if you want to use the functions outside the CloudyWebAPI module.
