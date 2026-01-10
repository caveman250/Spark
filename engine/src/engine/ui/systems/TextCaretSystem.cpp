#include "TextCaretSystem.h"

#include "Widgets.generated.h"
#include "engine/Application.h"
#include <easy/profiler.h>

#include "engine/ecs/util/SystemUtil.h"
#include "engine/ui/components/TextCaretComponent.h"
#include "engine/ui/components/WidgetComponent.h"

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

    void TextCaretSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("TextCaretSystem::OnUpdate");

        auto app = Application::Get();

        const auto& entities = updateData.GetEntities();
        auto* textCarets = updateData.GetComponentArray<components::TextCaretComponent>();
        auto* widgets = updateData.GetComponentArray<components::WidgetComponent>();

        ecs::util::ForEachEntity(this, updateData,
         [this, app, textCarets, widgets](size_t i)
        {
            auto& textCaret = textCarets[i];
            auto& widget = widgets[i];

            if (!textCaret.active)
            {
                widget.visibility = Visibility::Collapsed;
                return;
            }

            textCaret.currentStateTime += app->GetDeltaTime();
            if (textCaret.currentStateTime >= 0.5f)
            {
                widget.visibility = widget.visibility == Visibility::Visible ? Visibility::Collapsed : Visibility::Visible;
                textCaret.currentStateTime = 0.f;
            }
        });
    }
}
