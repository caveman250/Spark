#pragma once

#include "spark.h"

namespace se::editor::ui::asset_browser
{
    void SelectFile(const std::string& file);
    void OpenFile(const std::string& file);
    void SelectFolder(const std::string& folder);
}