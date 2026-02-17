#include "DeferredOpenGLOperations.h"

namespace se::render::opengl
{
    DeferredOpenGLOperations* DeferredOpenGLOperations::Get()
    {
        static DeferredOpenGLOperations* instance = new DeferredOpenGLOperations();
        return instance;
    }

    void DeferredOpenGLOperations::ExecuteDeferredOps()
    {
        for (const auto& op : m_DeferredOps)
        {
            op();
        }
        m_DeferredOps.clear();
    }
}
