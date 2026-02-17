#pragma once

#include "spark.h"

namespace se::threads::util
{
    void SetMainThread(std::thread::id id);
    bool IsMainThread();
}
