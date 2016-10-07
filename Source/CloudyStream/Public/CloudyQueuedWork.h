#pragma once

#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(CloudyQueuedWorkLog, Log, All)


class CloudyQueuedWork : public FNonAbandonableTask
{
    friend class FAutoDeleteAsyncTask<CloudyQueuedWork>;

public:
    CloudyQueuedWork(int FrameSize, uint32 *PixelBuffer, int i, TArray<TArray<FColor> > FrameBufferList,
                     TArray<int> PlayerFrameMapping, int ColIncInt, int PixelSize, int RowIncInt, TArray<FILE*> VideoPipeList) :
        TFrameSize(FrameSize),
        TPixelBuffer(PixelBuffer),
        Ti(i),
        TFrameBufferList(FrameBufferList),
        TPlayerFrameMapping(PlayerFrameMapping),
        TColIncInt(ColIncInt),
        TPixelSize(PixelSize),
        TRowIncInt(RowIncInt),
        TVideoPipeList(VideoPipeList)
    {}

    // This next section of code needs to be here.  Not important as to why.

    FORCEINLINE TStatId GetStatId() const
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(CloudyQueuedWork, STATGROUP_ThreadPoolAsyncTasks);
    }

//protected:
    int TFrameSize;
    uint32 *TPixelBuffer;
    int Ti;
    TArray<TArray<FColor> > TFrameBufferList;
    TArray<int> TPlayerFrameMapping;
    int TColIncInt;
    int TPixelSize;
    int TRowIncInt;
    TArray<FILE*> TVideoPipeList;

    void DoWork()
    {
        // Place the Async Code here.  This function runs automatically.
        //FColor Pixel;
        //for (int j = 0; j < TFrameSize; ++j) {
        //    Pixel = TFrameBufferList[TPlayerFrameMapping[Ti]][j];
        //    TPixelBuffer[j] = Pixel.DWColor();
        //}
        //
        //fwrite(TPixelBuffer, TColIncInt * TPixelSize, TRowIncInt, TVideoPipeList[Ti]);
    }

    
};