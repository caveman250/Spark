#pragma once
#include "spark.h"

namespace se::io
{
    class FileSystem
    {
    public:
        static std::string ReadTextFile(const std::string& path);
    };
}
