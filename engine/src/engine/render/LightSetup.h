#pragma once

#include "spark.h"
#include "PointLight.h"

namespace se
{
    struct LightSetup
    {
        std::vector<render::PointLight> pointLights;

        void Reset() { pointLights.clear(); }

        bool operator==(const LightSetup& rhs) const
        {
            return pointLights.size() == rhs.pointLights.size();
        }
    };
}
