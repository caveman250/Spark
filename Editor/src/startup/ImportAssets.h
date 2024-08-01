#pragma once
#include "StartupTask.h"

namespace se::editor::startup
{
    class ImportAssets : public StartupTask
    {
        void Run() override;
    private:
        void ProcessDir(const std::string& dir, const std::string& outputDir) const;
    };
}
