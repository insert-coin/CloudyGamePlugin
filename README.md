## Description

Plugin to interface between CloudyPanel and Engine

## Setup

In your game folder, create a folder named 'Plugins' if it doesn't exist. Put CloudyPanelPlugin in your Plugins folder. Build and run your game. The plugin should show up in Menu > Edit > Plugins.

## Usage
Can now capture and (very buggily) stream video via rtp, with no split screen. Run your game with plugin enabled, then open VLC and drag OtherFiles\test.sdp into VLC. If there are any issues with the sdp file, you can generate it by changing sstm in CloudyPanelPlugin.cpp, by including ' -sdp_file test.sdp ' around the front of the command string. Then delete the line 'SDP:' from test.sdp before you use it in VLC.

Note, all files from ffmpeg (output video, sdp file, log file out.txt etc) are probably generated in your Unreal Engine\Engine\Binaries\Win64 folder.
