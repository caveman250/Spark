#include "StartupManager.h"

namespace se::editor::startup
{
    void StartupManager::RunStartupTasks()
    {
        for (const auto& task : m_StartupTasks)
        {
            task->Run();
        }
    }
}
