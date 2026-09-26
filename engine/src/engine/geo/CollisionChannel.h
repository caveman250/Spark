#pragma once

#include "spark.h"
#include "engine/string/Symbol.h"

namespace se::geo
{
    struct CollisionChannel
    {
        SPARK_POD_CLASS()

        CollisionChannel(const string::Symbol& rhs) : channel(rhs) {}

        bool operator==(const CollisionChannel& rhs) const
        {
            return channel == rhs.channel;
        }

        SPARK_MEMBER(Serialized)
        string::Symbol channel;
    };
}
