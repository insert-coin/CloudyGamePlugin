## Description

Plugin to interface between CloudyPanel and Engine

## Setup

In your game folder, create a folder named 'Plugins' if it doesn't exist. Put CloudyPanelPlugin in your Plugins folder. Build and run your game. The plugin should show up in Menu > Edit > Plugins.

## Usage
Split screen 2 player is now implemented by default, by writing to file. Run your game with plugin enabled, split the screen with DebugCreatePlayer1 command, then open VLC and drag the generated output1.avi and output2.avi into VLC.

Note, all files from ffmpeg (output video, sdp file, log file out.txt etc) are probably generated in your Unreal Engine\Engine\Binaries\Win64 folder.
