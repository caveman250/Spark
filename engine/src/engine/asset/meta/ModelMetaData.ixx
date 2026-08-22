module;

#include "spark.h"

export module Spark.Asset.Meta.ModelMetaData;
import Spark.Asset.Meta.MetaData;

namespace se::asset::meta 
{
    export class ModelMetaData : public MetaData
    {
        SPARK_CLASS()

        ModelMetaData(const std::string& assetPath) : MetaData(assetPath) {};

        SPARK_MEMBER(Serialized)
        float importScale = 1.f;

        SPARK_MEMBER(Serialized)
        bool flipUVX = false;

        SPARK_MEMBER(Serialized)
        bool flipUVY = false;
    };
}
