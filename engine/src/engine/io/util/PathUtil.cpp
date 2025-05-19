#include "PathUtil.h"
#include "engine/string/util/StringUtil.h"
#include "engine/string/String.h"

namespace se::io::util
{
    void SplitPath(const String& path,
                   String& dir,
                   String& file,
                   String& extension)
    {
        String pathMinusExtension = {};
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