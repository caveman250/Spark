module;

#include "spark.h"
#include "engine/reflect/Object.h"

export module Spark.Asset.Asset;
import Spark.Asset.Meta.MetaData;

namespace se::asset
{
    export class Asset : public reflect::ObjectBase
    {
        SPARK_CLASS()

        virtual bool IsDataAsset() const { return false; }
        virtual bool UsesMetaData() const { return false; }
        virtual std::shared_ptr<meta::MetaData> CreateMetaData() const { return nullptr; }
        virtual std::string GetSourceFileExtension() const { return ".json"; }
        std::string GetName() const;

        SPARK_MEMBER(Serialized)
        std::string m_SourcePath = {};

        std::string m_Path = {};
    };
}
