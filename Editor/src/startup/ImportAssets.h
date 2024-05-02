#pragma once
#include "StartupTask.h"

namespace se::editor::startup
{
    class ImportAssets : public StartupTask
    {
        void Run() override;
    };
}
