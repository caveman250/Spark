#pragma once
#include "ToolWindow.h"
#include "engine/ecs/ecs_fwd.h"
#include "engine/geo/CollisionSettings.h"
#include "engine/geo/singleton_components/CollisionComponent.h"
#include "engine/string/Symbol.h"

namespace se::editor::ui
{
    class ProjectSettingsWindow : public ToolWindow
    {
    public:
        ProjectSettingsWindow(Editor* editor) : ToolWindow(editor) {}
        void Update() override;
        void ConstructUI() override;
        void DestroyUI() override;

    private:
        void CreateCollisionChannels();
        void CreateCollisionMatrix();

        ecs::Id m_InputBlocker;
        ecs::Id m_Window;
        ecs::Id m_ScrollBoxContent;
        std::vector<std::shared_ptr<properties::PropertyEditor>> m_PropertyEditors = {};
    };
}
