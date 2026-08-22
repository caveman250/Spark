module;

#include "spark.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/MouseInputComponent.h"

export module ScrollViewUpdateSystem;
import System;
import ScrollViewComponent;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    export class ScrollViewUpdateSystem : public ecs::EngineSystem
    {
    SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
