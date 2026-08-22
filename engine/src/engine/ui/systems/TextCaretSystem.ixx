module;

#include "spark.h"

export module TextCaretSystem;

import System;

namespace se::ui::systems
{
    class TextCaretSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
