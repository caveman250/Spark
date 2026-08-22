module;

#include "engine/string/util/StringUtil.h"

module Spark.Asset.Asset;

namespace se::asset
{
    std::string Asset::GetName() const
    {
        std::string dir, file;
        string::util::Split(m_Path, dir, file, '/', true);
        std::string extension;
        string::util::Split(file, file, extension, '.', true);

        return file;
    }
}
