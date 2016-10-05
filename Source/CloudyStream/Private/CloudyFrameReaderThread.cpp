#include "CloudyStreamPrivatePCH.h"

//***********************************************************
//Thread Worker Starts as NULL, prior to being instanced
//		This line is essential! Compiler error without it
CloudyFrameReaderThread* CloudyFrameReaderThread::Runnable = NULL;
//***********************************************************
 
DEFINE_LOG_CATEGORY(CloudyFrameReaderThreadLog)

CloudyFrameReaderThread::CloudyFrameReaderThread()
{
	Thread = FRunnableThread::Create(this, TEXT("CloudyFrameReaderThread"), 0, TPri_Normal); //windows default = 8mb for thread, could specify more
}
 
CloudyFrameReaderThread::~CloudyFrameReaderThread()
{
	delete Thread;
	Thread = NULL;
}
 
//Init
bool CloudyFrameReaderThread::Init()
{
	//Init the Data 
    UE_LOG(CloudyFrameReaderThreadLog, Warning, TEXT("Thread init"));
	return true;
}
 
//Run
uint32 CloudyFrameReaderThread::Run()
{
	//Initial wait before starting
	//FPlatformProcess::Sleep(0.03);
    UE_LOG(CloudyFrameReaderThreadLog, Warning, TEXT("Thread running"));
	
 
	//Run CloudyFrameReaderThread::Shutdown() from the timer in Game Thread that is watching
        //to see when CloudyFrameReaderThread::IsThreadFinished()
 
	return 0;
}
 
//stop
void CloudyFrameReaderThread::Stop()
{
    UE_LOG(CloudyFrameReaderThreadLog, Warning, TEXT("Thread stop"));
}
 
CloudyFrameReaderThread* CloudyFrameReaderThread::StartThread()// TArray<FColor> FrameBuffer, FReadSurfaceDataFlags flags, FIntRect Screen)
{
	//Create new instance of thread if it does not exist
	//		and the platform supports multi threading!
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new CloudyFrameReaderThread();			
	}
	return Runnable;
}
 
void CloudyFrameReaderThread::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}
 
void CloudyFrameReaderThread::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}
 
bool CloudyFrameReaderThread::IsThreadFinished()
{
	if(Runnable) 
        return Runnable->IsFinished();

	return true;
}
