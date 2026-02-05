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

        virtual bool UsesMetaData() const { return false; }
        virtual std::shared_ptr<meta::MetaData> CreateMetaData() const { return nullptr; }

        std::string m_Path = {};
    };
}
