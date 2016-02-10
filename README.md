## Description

Plugin to interface between CloudyPanel and Engine

## Setup

In your game folder, create a folder named 'Plugins' if it doesn't exist. Put CloudyPanelPlugin in your Plugins folder. Build and run your game. The plugin should show up in Menu > Edit > Plugins. 
To run split screen correctly, you need to modify Unreal Engine. Go to GameViewportClient.cpp and edit the function UGameViewportClient::LayoutPlayers(). Change SplitType to 4 player. Edit the code as follows:

	Comment out this line: const ESplitScreenType::Type SplitType = GetCurrentSplitscreenConfiguration();
	Add this line: const ESplitScreenType::Type SplitType = ESplitScreenType::FourPlayer;

## Usage
Split screen is now working. You will need to join game in the official way with CloudyPanel. For convenience, I have included my testing file OtherFiles/sendTCP.py, which you can use to join game. Plugin streams to HTTP, so use VLC to catch the frames. The addresses are:
ControllerId 1: http://localhost:8080,
ControllerId 2: http://localhost:8081
ControllerId 3: http://localhost:8082
ControllerId 4: http://localhost:8083

Note, all files from ffmpeg (output video, sdp file, log file out.txt etc) are probably generated in your Unreal Engine\Engine\Binaries\Win64 folder.
