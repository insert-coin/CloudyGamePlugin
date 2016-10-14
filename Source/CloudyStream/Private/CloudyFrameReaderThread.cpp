#include "CloudyStreamPrivatePCH.h"

//***********************************************************
//Thread Worker Starts as NULL, prior to being instanced
//		This line is essential! Compiler error without it
CloudyFrameReaderThread* CloudyFrameReaderThread::Runnable = NULL;
//***********************************************************
 
DEFINE_LOG_CATEGORY(CloudyFrameReaderThreadLog)

int TFrameSize;
uint32 *TPixelBuffer;
int Ti;
TArray<TArray<FColor> > TFrameBufferList;
TArray<int> TPlayerFrameMapping;
int TColIncInt;
int TPixelSize;
int TRowIncInt;
TArray<FILE*> TVideoPipeList;
int j;

CloudyFrameReaderThread::CloudyFrameReaderThread(int counter, int FrameSize, uint32 *PixelBuffer, int i, TArray<TArray<FColor> > FrameBufferList, TArray<int> PlayerFrameMapping, int ColIncInt, int PixelSize, int RowIncInt, TArray<FILE*> VideoPipeList)
{
    TFrameSize = FrameSize;
    TPixelBuffer = PixelBuffer;
    Ti = i;
    TFrameBufferList = FrameBufferList;
    TPlayerFrameMapping = PlayerFrameMapping;
    TColIncInt = ColIncInt;
    TPixelSize = PixelSize;
    TRowIncInt = RowIncInt;
    TVideoPipeList = VideoPipeList;
    
    // FRunnableThread::Create(FRunnable * InRunnable, const TCHAR* ThreadName, uint32 InStackSize, EThreadPriority InThreadPriority)
    Thread = FRunnableThread::Create(this, *FString::FromInt(counter), 0, TPri_Normal); // 0 is windows default = 8mb for thread, could specify more
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
    j = 0;
	
    //UE_LOG(CloudyFrameReaderThreadLog, Warning, TEXT("Thread init"));
	return true;
}
 
//Run
uint32 CloudyFrameReaderThread::Run()
{
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);

    //UE_LOG(CloudyFrameReaderThreadLog, Warning, TEXT("Thread running"));

    FColor Pixel;
    for (j = 0; j < TFrameSize; ++j) {
        Pixel = TFrameBufferList[TPlayerFrameMapping[Ti]][j]; 
        TPixelBuffer[j] = Pixel.DWColor();

        //prevent thread from using too many resources
        //FPlatformProcess::Sleep(0.01); // Streaming not working with this
    }

    fwrite(TPixelBuffer, TColIncInt * TPixelSize, TRowIncInt, TVideoPipeList[Ti]);

	//Run CloudyFrameReaderThread::Shutdown() from the timer in Game Thread that is watching
    //to see when CloudyFrameReaderThread::IsThreadFinished()
 
	return 0;
}
 
//stop
void CloudyFrameReaderThread::Stop()
{
    //UE_LOG(CloudyFrameReaderThreadLog, Warning, TEXT("Thread stop"));
}
 
CloudyFrameReaderThread* CloudyFrameReaderThread::StartThread(int counter, int FrameSize, uint32 *PixelBuffer, int i, TArray<TArray<FColor> > FrameBufferList, TArray<int> PlayerFrameMapping, int ColIncInt, int PixelSize, int RowIncInt, TArray<FILE*> VideoPipeList)
{
	//Create new instance of thread if it does not exist
	//		and the platform supports multi threading!
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
        Runnable = new CloudyFrameReaderThread(counter, FrameSize, PixelBuffer, i, FrameBufferList, PlayerFrameMapping, ColIncInt, PixelSize, RowIncInt, VideoPipeList);
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

bool CloudyFrameReaderThread::IsFinished()
{
    return j >= TFrameSize;
}
