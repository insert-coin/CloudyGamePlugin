#pragma once

#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(CloudyFrameReaderThreadLog, Log, All)

//~~~~~ Multi Threading ~~~
class CloudyFrameReaderThread : public IModuleInterface, public FRunnable
{	
	/** Singleton instance, can access the thread any time via static accessor, if it is active! */
	static  CloudyFrameReaderThread* Runnable;
 
	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;
 
public:

 
	//Done?
	bool IsFinished() const
	{
        return false;
	}
 
	//~~~ Thread Core Functions ~~~
 
	//Constructor / Destructor
	CloudyFrameReaderThread();
	virtual ~CloudyFrameReaderThread();
 
	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface
 
	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();
 
 
 
	//~~~ Starting and Stopping Thread ~~~
 
 
 
	/* 
		Start the thread and the worker from static (easy access)! 
		This code ensures only 1 Prime Number thread will be able to run at a time. 
		This function returns a handle to the newly started instance.
	*/
    static CloudyFrameReaderThread* StartThread();// TArray<FColor> FrameBuffer, FReadSurfaceDataFlags flags, FIntRect Screen);
 
	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();
    static bool IsThreadFinished();
 
};