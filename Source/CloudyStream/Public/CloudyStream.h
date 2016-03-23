// Some copyright should be here...

#pragma once

#include "ModuleManager.h"

#include "HttpRequestAdapter.h"
#include "HttpModule.h"
#include "IHttpResponse.h"
#include "Http.h"

DECLARE_LOG_CATEGORY_EXTERN(CloudyStreamLog, Log, All)

class CloudyStreamImpl : public IModuleInterface
{
public:

	/** Methods **/

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	void CloudyStreamImpl::SetUpVideoCapture();
	void CloudyStreamImpl::SetUpPlayer(int ControllerId);
	void CloudyStreamImpl::StreamFrameToClient();
	// Only handle 4 player split screen for current solution
	void CloudyStreamImpl::Split4Player();

	virtual void CloudyStreamImpl::StartPlayerStream(int32 ControllerId);
	virtual void CloudyStreamImpl::StopPlayerStream(int32 ControllerId);


	/**
	* Singleton-like access to this module's interface.  This is just for convenience!
	* Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
	static inline CloudyStreamImpl& Get()
	{
		return FModuleManager::LoadModuleChecked< CloudyStreamImpl >("CloudyStream");
	}

	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("CloudyStream");
	}

	
	// Timer for capturing frames
	bool CloudyStreamImpl::CaptureFrame(float DeltaTime);


	/** Class variables **/
	int NumberOfPlayers;
	TArray<FILE*> VideoPipeList;
	TArray<TArray<FColor> > FrameBufferList;
	bool isEngineRunning;
	int sizeX, sizeY, halfSizeX, halfSizeY;
	TArray<int> PlayerFrameMapping; // index is frame index, value is player index
	FIntRect Screen1, Screen2, Screen3, Screen4;
	FReadSurfaceDataFlags flags;

};