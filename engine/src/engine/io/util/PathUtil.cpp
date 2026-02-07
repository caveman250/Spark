#include "PathUtil.h"
#include "engine/string/util/StringUtil.h"

namespace se::io::util
{
    void SplitPath(const std::string& path,
                   std::string& dir,
                   std::string& file,
                   std::string& extension)
    {
        std::string fileName = {};
        if (!string::util::Split(path, dir, fileName, '/', true))
        {
            if (fileName.starts_with(".") && fileName.find_last_of(".") == 0)
            {
                extension = {};
            }
            else if (!string::util::Split(fileName, file, extension, '.', true))
            {
                file = fileName;
                extension = {};
            }

            return;
        }

        if (fileName.starts_with(".") && fileName.find_last_of(".") == 0)
        {
            file = fileName;
            extension = {};
        }
        else if (!string::util::Split(fileName, file, extension, '.', true))
        {
            file = fileName;
            extension = {};
        }
    }
}