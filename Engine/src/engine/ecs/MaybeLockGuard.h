#pragma once

#include "spark.h"
#include "UpdateMode.h"

namespace se::ecs
{
    struct MaybeLockGuard
    {
        [[nodiscard]] MaybeLockGuard(UpdateMode::Type mode, std::mutex* mutex)
                : m_Mode(mode)
                , m_Mutex(mutex)
        {
            if (m_Mode == UpdateMode::MultiThreaded)
            {
                m_Mutex->lock();
            }
        }

        ~MaybeLockGuard()
        {
            if (m_Mode == UpdateMode::MultiThreaded)
            {
                m_Mutex->unlock();
            }
        }

        UpdateMode::Type m_Mode;
        std::mutex* m_Mutex;
    };
}