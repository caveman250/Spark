#pragma once 

#include "spark.h"
#include "MetaData.h"
#include "engine/asset/texture/Format.h"
#include "engine/math/Vec3.h"

namespace se::asset::meta 
{
    class ModelMetaData : public MetaData
    {
        SPARK_CLASS()

        ModelMetaData(const std::string& assetPath) : MetaData(assetPath) {};

        SPARK_MEMBER(Serialized)
        float importScale = 1.f;
    };
}
