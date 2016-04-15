# CloudyGamePlugin
## Description

Unreal Engine 4 Plugin for CloudyGame.
This plugin contains 5 modules:

* CloudyWebAPI
* CloudyRemoteController
* CloudyStream
* CloudyPlayerManager
* CloudySaveManager

## Plugin Setup

We assume that you are using our Unreal Engine fork of the source code, and have successfully built and compiled the engine. If not, please refer to the Unreal Engine fork [here](https://github.com/insert-coin/UnrealEngine).

0. You should have one Unreal Engine game project created. To do this, open the Unreal Engine executable which can be found at `UnrealEngine\Engine\Binaries\Win64`. Launch `UE4Editor.exe`. 
  - If this is your first run, you should see a window to create a game. Ensure that the circled parts are selected correctly.
    - We recommended using the "First Person" template, but you are free to use others (except "Basic Code").
    - If your computer has a weak GPU, you can reduce the graphical quality of the game (3rd circle from the top, middle square button).
    - Please remember the location and name of the project.
    - ![Create Game](http://i.imgur.com/kj8HO4K.png)
  - If you do not see the above window, then the engine has launched with a previously made game.
    - Click on "File", then click on "New Project". You can now continue with the steps above.

1. In your game folder, create a folder named "Plugins" if it doesn't exist. Put CloudyGamePlugin in your Plugins folder. Build and run your game (details in step 6). The plugin should show up in Menu > Edit > Plugins.

  Your game directory structure should look similar to this (assuming the name of your game is MyProject):
  ```bash
  MyProject
  ├───Binaries
  ├───Build
  ├───Config
  ├───Content
  ├───DerivedDataCache
  ├───Intermediate
  ├───Plugins
  │   └───CloudyGamePlugin
  │       ├───OtherFiles
  │       ├───Resources
  │       └───Source
  │           ├───CloudyPlayerManager
  │           ├───CloudyRemoteController
  │           ├───CloudySaveManager
  │           ├───CloudyStream
  │           └───CloudyWebAPI
  ├───Saved
  └───Source
  ```
  Some folders may be missing if you have not compiled your project before.

2. Download the precompiled FFmpeg build from [here](https://ffmpeg.zeranoe.com/builds/). Choose the static build. As the file is compressed using 7z, you may need to download 7zip [here](http://www.7-zip.org/download.html) to be able to unzip the downloaded file.

  Put `ffmpeg.exe` into your `Unreal Engine\Engine\Binaries\Win64` folder. This is the Unreal Engine source code, not your game project.

3. Modify Unreal Engine. If you have downloaded the `cloudygame` branch of our Unreal Engine fork, this step can be skipped. 
  
  Go to `UGameViewportClient.cpp` (found at `UnrealEngine/Engine/Source/Runtime/Engine/Private/`) and edit the function `UGameViewportClient::LayoutPlayers()` at line 1587. Change SplitType to 4 player. To do this, edit the code as follows:
  
  Comment out this line: `const ESplitScreenType::Type SplitType = GetCurrentSplitscreenConfiguration();` (on line 1590)
  
  Add this line: `const ESplitScreenType::Type SplitType = ESplitScreenType::FourPlayer;`

4. Add the following system environment variables. To learn how to do this, visit this webpage [here](http://www.howtogeek.com/118594/how-to-edit-your-system-path-for-easy-command-line-access/).
  - Variable: `ROBOT_USER`. Value: `username; password`. Replace the username and password with the actual values. Do not use a semicolon (`;`) in the username or password. 
    - The robot user has to be an administrator in CloudyWeb. Do this by creating a super user (instructions in the [CloudyWeb repository](https://github.com/insert-coin/cloudyweb)).
  - Variable: `CLOUDYWEB_URL`. Value: `http://url:port`. Replace the URL and port with the actual address where [CloudyWeb](https://github.com/insert-coin/cloudyweb) is deployed.
    - To test the deployment locally, you can use `http://127.0.0.1:8000` as the value.

5. Compile Unreal Engine with the plugins. Go to your game project folder, and right click the `.uproject` file. Click "Generate Visual Studio project files". 

  ![Right click .uproject](http://i.imgur.com/ou3xukU.png)
  
  Once done, open the `.sln` file. Then, in the solution explorer, you should see 3 folders: Engine, Games, Programs. Expand the "Games" folder, right-click your game project, and click "Build". 
  ![Build](http://i.imgur.com/6yGUQud.png)

6. If you are installing this plugin as a user, then you are good to go. The information after this step is for game developers. To run the game, double click on the `.uproject` file from the previous step. You can also right-click the `.uproject` file and click on "Launch game" to run it without the Unreal Engine interface.

# CloudyPlayerManager
## Usage

This plugin currently supports join game and quit game. To test, set up CloudyWeb (if necessary) with a new game, and send the following sample JSON packets via TCP to <your public IP>:55556 :

To join game:
```json
{
  "controller": "0",
  "streaming_port": "30000",
  "streaming_ip": "127.0.0.1",
  "game_id": "1",
  "username": "abc",
  "game_session_id": "1",
  "command": "join"
}
```

To quit game:
```json
{
  "controller": "0",
  "command": "quit"
}
```

`OtherFiles/sendTCP.py` has been included to assist testing.

# CloudyStream
## Description

Module for broadcasting the video stream.

## Usage

Open streams (in VLC, the Thin Client, or other media players) using the following addresses:

Player 0: `http://<your public IP>:30000`

Player 1: `http://<your public IP>:30001`

Player 2: `http://<your public IP>:30002`

Player 3: `http://<your public IP>:30003`


# CloudySaveManager
## Description

Module to provide customized save game and load game API. This API will allow the game developer to use our custom save/load game functions to upload the player's save game file to our cloud.

## Usage
- In `YourProject/Source/YourProject/YourProject.Build.cs`:
  - Ensure that `CloudySaveManager` is added to your `PrivateDependencyModuleNames`. 
  - E.g. `PrivateDependencyModuleNames.AddRange(new string[] { "CloudySaveManager" });`

- In your .cpp file where you want to use our custom `Cloudy_SaveGameToSlot` functions: 
  - Ensure that `#include "ICloudySaveManager.h"` is included.

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

## Usage
- In the .cpp file where you want to use any public functions in this module: 
  - Ensure that `#include "../../CloudyWebAPI/Public/ICloudyWebAPI.h"` is included.

Assuming that we want to use the `UploadFile` function from this module, we can call the function this way:

```cpp
ICloudyWebAPI::Get().UploadFile(Filename, PlayerControllerId);
```

### Adding more API
- `CloudyWebAPI.cpp` contains all the function logic. Do your work here.
- `CloudyWebAPI.h` contains all the function declaration. Declare all your functions here.
- `ICloudyWebAPI.h` contains public function declarations. Only declare functions here if you want to use the functions outside the CloudyWebAPI module.
