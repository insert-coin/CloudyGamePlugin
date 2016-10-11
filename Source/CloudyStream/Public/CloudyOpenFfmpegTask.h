#pragma once

#include "ModuleManager.h"
#include <sstream>
#include "CloudyStream.h"

DECLARE_LOG_CATEGORY_EXTERN(CloudyOpenFfmpegTaskLog, Log, All)


class CloudyOpenFfmpegTask : public FNonAbandonableTask
{
    friend class FAutoDeleteAsyncTask<CloudyOpenFfmpegTask>;

public:
    CloudyOpenFfmpegTask(std::stringstream *StringStream) :
        TStringStream(StringStream)
    {}

    // This next section of code needs to be here.  Not important as to why.

    FORCEINLINE TStatId GetStatId() const
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(CloudyOpenFfmpegTask, STATGROUP_ThreadPoolAsyncTasks);
    }

//protected:
    std::stringstream *TStringStream;

    void DoWork()
    {
        // Place the Async Code here.  This function runs automatically.
        FILE *f = _popen(TStringStream->str().c_str(), "wb");
        CloudyStreamImpl::AddPipeToList(f);
    }

    
};