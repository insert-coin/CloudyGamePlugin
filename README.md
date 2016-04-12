# CloudyGamePlugin
## Description

Unreal Engine 4 Plugins for CloudyGame.
This plugin contains 5 modules:

* CloudyWebAPI
* CloudyRemoteController
* CloudyStream
* CloudyPlayerManager
* CloudySaveManager

## Setup

In your game folder, create a folder named 'Plugins' if it doesn't exist. Put CloudyGamePlugin in your Plugins folder. Build and run your game. The plugin should show up in Menu > Edit > Plugins.

## Usage

This plugin currently supports join game and quit game. To test, set up CloudyWeb (if necessary) with a new game, and send the following sample JSON packets via TCP to <your public IP>:55556 :

To join game - {"controller": "0", "streaming_port": "30000", "streaming_ip": "127.0.0.1", "game_id": "2", "username": "abc", "game_session_id": "3", "command": "join"}

To quit game - {"controller": "0", "command": "quit"}

OtherFiles/sendTCP.py has been included to assist testing.

# CloudyStream
## Description

Module for streaming

## Setup

Put ffmpeg executable into your Unreal Engine\Engine\Binaries\Win64 folder.

Modify Unreal Engine. Go to UGameViewportClient.cpp and edit the function UGameViewportClient::LayoutPlayers(). Change SplitType to 4 player. Edit the code as follows:

Comment out this line: const ESplitScreenType::Type SplitType = GetCurrentSplitscreenConfiguration();

Add this line: const ESplitScreenType::Type SplitType = ESplitScreenType::FourPlayer;

## Usage

Open streams (in vlc/ThinClient etc media players) using the following addresses:

Player 0: http://\<your public IP\>:30000

Player 1: http://\<your public IP\>:30001

Player 2: http://\<your public IP\>:30002

Player 3: http://\<your public IP\>:30003


# CloudySaveManager
## Description

Module to provide customized save game and load game API. This API will allow the game developer to use our custom save/load game functions to upload the player's save game file to our cloud.

## Setup
- Assume that the CloudyGamePlugin has been successfully installed. If not, please read the setup instructions at the top of this readme.

- In `YourProject/Source/YourProject/YourProject.Build.cs`:
  - Ensure that `CloudySaveManager` is added to your `PrivateDependencyModuleNames`. 
  - E.g. `PrivateDependencyModuleNames.AddRange(new string[] { "CloudySaveManager" });`

- In your .cpp file where you want to use our custom `Cloudy_SaveGameToSlot` functions: 
  - Ensure that `#include "ICloudySaveManager.h"` is included.

## Usage
### Save Game
`Cloudy_SaveGameToSlot` takes in the same three functions as Unreal Engine's `SaveGameToSlot`, with an additional parameter: the player controller index.

API:
```cpp
UFUNCTION(BlueprintCallable, Category="Game")
virtual bool Cloudy_SaveGameToSlot
(
    USaveGame * SaveGameObject, // The save game object
    const FString & SlotName,   // The name of the save file
    const int32 UserIndex,
    const int32 PCID,           // Player Controller ID of the player you are saving
)
```
Example: 
```cpp
#include "ICloudySaveManager.h"

// Create a save game object
UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
// Save the game, and upload to our cloud
ICloudySaveManager::Get().Cloudy_SaveGameToSlot(SaveGameInstance, "SaveGame1", SaveGameInstance->UserIndex, 0);
```
### Load Game
`Cloudy_LoadGameFromSlot` takes in the same three functions as Unreal Engine's `LoadGameFromSlot`, with an additional parameter: the player controller index.

API:
```cpp
UFUNCTION(BlueprintCallable, Category="Game")
USaveGame* Cloudy_LoadGameFromSlot
(
    const FString& SlotName,  // The name of the save file
    const int32 UserIndex,
    const int32 PCID          // Player Controller ID of the player you are saving
)
````

Additionally, before loading the data from the save file, please check if the save file has been successfully downloaded from our cloud.

For example:
```cpp
// Create the save game object
UMySaveGame* LoadGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
// Attempt to load the save game from our cloud
LoadGameInstance = Cast<UMySaveGame>(ICloudySaveManager::Get().Cloudy_LoadGameFromSlot(TEXT("SaveGame1"), 
                                                                                       LoadGameInstance->UserIndex, 0));
// Check if the save file has been loaded successfully
if (LoadGameInstance != NULL)
{
    //Load your data here
}
```

# CloudyWebAPI
## Description

Module to provide network API for communication to the CloudyWeb server.

## Setup
- It is assumed that you have the CloudyGamePlugin successfully installed. If not, please read the setup instructions at the top of this readme.

- In the .cpp file where you want to use any public functions in this module: 
  - Ensure that `#include "../../CloudyWebAPI/Public/ICloudyWebAPI.h"` is included.
  
- Add the following system environment variables
  - Variable: `ROBOT_USER`. Value: `username; password`. Replace the username and password with the actual values. Do not use a semicolon (`;`) in the username or password.
  - Variable: `CLOUDYWEB_URL`. Value: `http://url:port`. Replace the URL and port with the actual values.
  
## Usage
Assuming that we want to use the `UploadFile` function from this module, we can call the function this way:

```cpp
ICloudyWebAPI::Get().UploadFile(Filename, PlayerControllerId);
```

### Adding more API
- `CloudyWebAPI.cpp` contains all the function logic. Do your work here.
- `CloudyWebAPI.h` contains all the function declaration. Declare all your functions here.
- `ICloudyWebAPI.h` contains public function declarations. Only declare functions here if you want to use the functions outside the CloudyWebAPI module.
