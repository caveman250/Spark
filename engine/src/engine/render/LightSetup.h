#pragma once

#include "spark.h"
#include "DirLight.h"
#include "PointLight.h"

namespace se
{
    struct LightSetup
    {
        std::vector<render::DirLight> dirLights;
        std::vector<render::PointLight> pointLights;

        void Reset()
        {
            dirLights.clear();
            pointLights.clear();
        }

        bool operator==(const LightSetup& rhs) const
        {
            return dirLights.size() == rhs.dirLights.size() && pointLights.size() == rhs.pointLights.size();
        }
    };
}
