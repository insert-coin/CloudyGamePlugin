#pragma once

#define CLOUDYGAME_REMOTE_CONTROLLER_SERVER_DEFAULT_ENDPOINT FIPv4Endpoint(FIPv4Address(0, 0, 0, 0), 55555)

#include "Core.h"
#include "CoreUObject.h"
#include "ModuleManager.h"
#include "Networking.h"
#include "Engine.h"


#include "UdpRemoteControllerSegment.h"
#include "RemoteControllerServer.h"