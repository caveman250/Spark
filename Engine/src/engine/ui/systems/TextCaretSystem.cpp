#include "TextCaretSystem.h"

#include "Widgets.generated.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/components/TextCaretComponent.h"
#include "engine/ui/components/WidgetComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void TextCaretSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("TextCaretSystem::OnUpdate")

        auto app = Application::Get();

        const auto& entities = updateData.GetEntities();
        auto* textCarets = updateData.GetComponentArray<components::TextCaretComponent>();
        auto* widgets = updateData.GetComponentArray<components::WidgetComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& textCaret = textCarets[i];
            auto& widget = widgets[i];

            if (!textCaret.active)
            {
                widget.renderingEnabled = false;
                continue;
            }

            textCaret.currentStateTime += app->GetDeltaTime();
            if (textCaret.currentStateTime >= 0.5f)
            {
                widget.renderingEnabled = !widget.renderingEnabled;
                textCaret.currentStateTime = 0.f;
            }
        }
    }
}
