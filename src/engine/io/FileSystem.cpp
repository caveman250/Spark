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

    char* FileSystem::ReadBinaryFile(const std::string& path)
    {
        std::ifstream myfile;
        myfile.open (path, std::ios::binary | std::ios::ate);
        std::streamsize size = myfile.tellg();
        myfile.seekg(0, std::ios::beg);
        char* data = static_cast<char*>(std::malloc(size));
        myfile.read(data, size);
        myfile.close();

        return data;
    }

    void FileSystem::WriteBinaryFile(const std::string& path, char* data, size_t size)
    {
        logging::Log::Error(std::to_string(std::stacktrace::current()));

        std::ofstream myfile;
        myfile.open (path, std::ios::binary);
        myfile.write(data, size);
        myfile.close();
    }
}
