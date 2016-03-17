# CloudyPanelPlugin
## Description

Plugin to interface between CloudyPanel and Engine. *NEW* CloudyPanel interface and streaming functions have been separated into different plugins.

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

Note, all files from ffmpeg (output video, sdp file, log file out.txt etc) are probably generated in your Unreal Engine\Engine\Binaries\Win64 folder.