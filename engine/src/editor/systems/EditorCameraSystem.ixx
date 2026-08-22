export module EditorCameraSystem;

import System;

using namespace se;

namespace se::editor::systems
{
    export class EditorCameraSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults& results) override;
    };

}
