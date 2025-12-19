#include "PathUtil.h"
#include "engine/string/util/StringUtil.h"

namespace se::io::util
{
    void SplitPath(const std::string& path,
                   std::string& dir,
                   std::string& file,
                   std::string& extension)
    {
        std::string pathMinusExtension = {};
        if (!string::util::Split(path, pathMinusExtension, extension, '.', true))
        {
            if (!string::util::Split(path, dir, file, '/', true))
            {
                dir = path;
                file = {};
                extension = {};
            }

            return;
        }

        if (!string::util::Split(pathMinusExtension, dir, file, '/', true))
        {
            dir = {};
            file = path;
        }
    }
}