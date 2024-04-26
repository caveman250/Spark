#pragma once
#include "spark.h"

namespace se::io
{
    class FileSystem
    {
    public:
        static std::string ReadTextFile(const std::string& path);
        static char* ReadBinaryFile(const std::string& path);
        static void WriteBinaryFile(const std::string& path, char* data, size_t size);
    };
}
