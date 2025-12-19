#pragma once

#include "spark.h"

namespace se::io::util
{
    void SplitPath(const std::string& path, std::string& dir, std::string& file, std::string& extension);
}
