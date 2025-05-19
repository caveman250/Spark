#pragma once

#include "spark.h"

namespace se::io::util
{
    void SplitPath(const String& path, String& dir, String& file, String& extension);
}
