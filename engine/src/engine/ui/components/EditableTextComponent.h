#pragma once

#include "spark.h"
#include "engine/asset/font/Font.h"
#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/IndexBuffer.h"
#include "engine/render/MaterialInstance.h"
#include "engine/ui/util/MeshUtil.h"

namespace se::ui::systems
{
    class TextRenderSystem;
}

namespace se::ui::components
{
    struct EditableTextComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        // text
        asset::AssetReference<asset::Font> font = { };
        int fontSize = 0;
        std::string text = { };
        text::Alignment alignment = text::Alignment::Left;
        text::WrapMode wrap = text::WrapMode::None;

        // edit mode
        bool enabled = true;
        bool inEditMode = false;
        std::string editText = { };
        int caretPosition = 0;
        ecs::Signal<std::string> onComitted = {};
        ecs::Signal<int> onCaretMoved = {};

        // internal
        std::shared_ptr<render::MaterialInstance> materialInstance = { };
        std::shared_ptr<render::VertexBuffer> vertBuffer = { };
        std::shared_ptr<render::IndexBuffer> indexBuffer = { };
        int lastFontSize = { };
        std::string lastText = { };
    };
}

#include "engine/ui/DesiredSizeCalculator.h"

namespace se::ui
{
    template<>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::EditableTextComponent>(ecs::System*,
                                                                                                  [[maybe_unused]] const ecs::Id& entity,
                                                                                                  components::RectTransformComponent& thisRect,
                                                                                                  const components::EditableTextComponent* text)
    {
        auto window = Application::Get()->GetWindow();
        auto ret = util::MeasureText(thisRect.rect,
                                         text->font.GetAsset(),
                                         static_cast<int>(text->fontSize * window->GetContentScale()),
                                         text->inEditMode ? text->editText : text->text,
                                         true,
                                         text->wrap);
        return ret;
    }
}
