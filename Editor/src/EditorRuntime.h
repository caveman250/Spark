#pragma once
#include "startup/StartupManager.h"

namespace se::editor
{
    class EditorRuntime
    {
    public:
        void Init();
        void Update();
        void Shutdown();

    private:
        startup::StartupManager m_StartupManager;
    };
}
