#pragma once 

#include "spark.h"

namespace se::ecs 
{
    class System;

    struct SystemRecord
    {
        reflect::Class* type = nullptr;
        System* instance = nullptr;
    };
}