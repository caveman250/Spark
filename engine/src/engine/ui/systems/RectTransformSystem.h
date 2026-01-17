#pragma once

#include "spark.h"
#include "engine/ecs/System.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class RectTransformSystem : public ecs::EngineSystem
    {
    SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        ecs::UpdateMode GetUpdateMode() const override;
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
