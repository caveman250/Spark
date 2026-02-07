#pragma once

#include "spark.h"
#include "engine/reflect/Object.h"
#include "engine/reflect/Reflect.h"

namespace se::asset::meta
{
    class MetaData;
}

namespace se::asset
{
    class Asset : public reflect::ObjectBase
    {
        SPARK_CLASS()

        virtual bool IsDataAsset() const { return false; }
        virtual bool UsesMetaData() const { return false; }
        virtual std::shared_ptr<meta::MetaData> CreateMetaData() const { return nullptr; }
        virtual std::string GetSourceFileExtension() const { return ".json"; }

        SPARK_MEMBER(Serialized)
        std::string m_SourcePath = {};

        std::string m_Path = {};
    };
}
