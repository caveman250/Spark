module;

#include <easy/profiler.h>
#include "engine/ui/components/TextCaretComponent.h"
#include "engine/ui/components/WidgetComponent.h"

module TextCaretSystem;
import SystemUtil;
import Application;
import Widgets;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration TextCaretSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Text Caret System")
                    .WithComponent<components::TextCaretComponent>()
                    .WithComponent<components::WidgetComponent>();
    }

    void TextCaretSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("TextCaretSystem::OnUpdate");

        auto app = Application::Get();
        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [app](const ecs::SystemUpdateData& updateData)
        {
            auto* textCarets = updateData.GetComponentArray<components::TextCaretComponent>();
            auto* widgets = updateData.GetComponentArray<components::WidgetComponent>();

            for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
            {
                auto& textCaret = textCarets[i];
                auto& widget = widgets[i];

                if (!textCaret.active)
                {
                    widget.visibility = Visibility::Collapsed;
                    continue;
                }

                textCaret.currentStateTime += app->GetDeltaTime();
                if (textCaret.currentStateTime >= 0.5f)
                {
                    widget.visibility = widget.visibility == Visibility::Visible ? Visibility::Collapsed : Visibility::Visible;
                    textCaret.currentStateTime = 0.f;
                }
            }
        });
    }
}
