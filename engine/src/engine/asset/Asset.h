#pragma once

#include "spark.h"
#include "engine/reflect/Object.h"
#include "engine/reflect/Reflect.h"

namespace se::asset
{
    class Asset : public reflect::ObjectBase
    {
        SPARK_CLASS()

        std::string m_Path = {};
    };
}
