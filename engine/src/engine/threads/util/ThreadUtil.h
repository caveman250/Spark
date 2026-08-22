#pragma once

#include "spark.h"
#include <thread>

namespace se::threads::util
{
    void SetMainThread(std::thread::id id);
    bool IsMainThread();
}
