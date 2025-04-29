#include "PathUtil.h"
#include "engine/string/util/StringUtil.h"

namespace se::io::util
{
    void SplitPath(const String& path,
                   String& dir,
                   String& file,
                   String& extension)
    {
        String pathMinusExtension = {};
        string::util::Split(path, pathMinusExtension, extension, '.', true);
        SPARK_ASSERT(extension.Size() > 0);
        if (!string::util::Split(pathMinusExtension, dir, file, '/', true))
        {
            dir = {};
            file = path;
        }
    }
}