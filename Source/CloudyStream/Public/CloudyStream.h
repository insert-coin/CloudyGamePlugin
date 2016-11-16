// Some copyright should be here...

#pragma once

#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(CloudyStreamLog, Log, All)

class CloudyStreamImpl : public IModuleInterface
{
public:

    /** Methods **/

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /**
    * One-time set up for variables used during streaming, including frame
    * dimensions and split-screen information
    */
    void SetUpVideoCapture();

    /**
    * Starts a stream for a player. This is called by CloudyPlayerManager
    * (another module). It can be accessed by other modules.
    *
    * @param ControllerId The Controller ID of the player to start streaming
    */
    virtual void StartPlayerStream(int32 ControllerId);

    /**
    * Stops a player's stream and clean up. This is called by CloudyPlayerManager
    * (another module). It can be accessed by other modules.
    *
    * @param ControllerId The Controller ID of the player to start streaming
    */
    virtual void StopPlayerStream(int32 ControllerId);


    /**
    * Singleton-like access to this module's interface.  This is just for
    * convenience! Beware of calling this during the shutdown phase, though.
    * Your module might have been unloaded already.
    *
    * @return Returns singleton instance, loading the module on demand if needed
    */
    static inline CloudyStreamImpl& Get()
    {
        return FModuleManager::LoadModuleChecked< CloudyStreamImpl >("CloudyStream");
    }

    /**
    * Checks to see if this module is loaded and ready.  It is only valid to call
    * Get() if IsAvailable() returns true.
    *
    * @return True if the module is loaded and ready to use
    */
    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("CloudyStream");
    }


    // Timer for capturing frames
    bool CaptureFrame(float DeltaTime);

    /** Class variables **/
    int NumberOfPlayers;
    bool isEngineRunning;
    int sizeX, sizeY;
    float RowIncrement, ColIncrement;
    int RowIncInt, ColIncInt;
    TArray<FIntRect> ScreenList;
    FReadSurfaceDataFlags flags; // needed to read buffer from engine
};
