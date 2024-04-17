#include "FileSystem.h"

#include "engine/logging/Log.h"

namespace se::io
{
    std::string FileSystem::ReadTextFile(const std::string& path)
    {
        std::string fileContents;
        std::ifstream fileStream(path, std::ios::in);
        if (fileStream.is_open())
        {
            std::stringstream sstr;
            sstr << fileStream.rdbuf();
            fileContents = sstr.str();
            fileStream.close();
        }
        else
        {
            logging::Log::Error("Cannot open file {0}!", path.c_str());
        }

        return fileContents;
    }
}
