#pragma once

#include "spark.h"

namespace se::render::opengl
{
    class DeferredOpenGLOperations
    {
    public:
        static DeferredOpenGLOperations* Get();
        template<typename Func>
        void AddDeferredOp(Func&& func)
        {
            auto guard = std::lock_guard(m_Mutex);
            m_DeferredOps.push_back(std::forward<Func>(func));
        }
        void ExecuteDeferredOps();
    private:
        std::mutex m_Mutex = {};
        std::vector<std::function<void()>> m_DeferredOps = {};
    };
}