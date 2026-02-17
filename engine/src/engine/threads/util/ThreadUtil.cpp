#include "ThreadUtil.h"

namespace se::threads::util
{
    static std::thread::id MainThreadId = {};

    void SetMainThread(std::thread::id id)
    {
        MainThreadId = id;
    }

    bool IsMainThread()
    {
        return std::this_thread::get_id() == MainThreadId;
    }
}
