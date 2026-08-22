#pragma once

#include "ImportAssets.h"
#include "StartupTask.h"

namespace se::editor::startup
{
    class StartupManager
    {
    public:
        void RunStartupTasks();

    private:
        std::array<std::unique_ptr<StartupTask>, 1> m_StartupTasks =
        {
            std::make_unique<ImportAssets>()
        };
    };
}
