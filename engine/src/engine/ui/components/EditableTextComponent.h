#pragma once

#include "spark.h"
#include "engine/asset/font/Font.h"
#include "engine/asset/texture/Texture.h"
#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/IndexBuffer.h"
#include "engine/render/MaterialInstance.h"
#include "engine/string/String.h"
#include "engine/ui/Rect.h"
#include "engine/ui/util/MeshUtil.h"
#include "engine/ui/util/RectTransformUtil.h"

namespace se::ui::systems
{
    class TextRenderSystem;
}

namespace se::ui::components
{
    struct EditableTextComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        // text
        std::shared_ptr<asset::Font> font = { };
        int fontSize = 0;
        String text = { };
        text::Alignment::Type alignment = text::Alignment::Left;
        text::WrapMode::Type wrap = text::WrapMode::None;

        // edit mode
        bool inEditMode = false;
        String editText = { };
        int caretPosition = 0;
        ecs::Signal<String> onComitted = {};
        ecs::Signal<int> onCaretMoved = {};

        // internal
        std::shared_ptr<render::MaterialInstance> materialInstance = { };
        std::shared_ptr<render::VertexBuffer> vertBuffer = { };
        std::shared_ptr<render::IndexBuffer> indexBuffer = { };
        int lastFontSize = { };
        String lastText = { };
    };
}

#include "engine/ui/DesiredSizeCalculator.h"

namespace se::ui
{
    template<>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::EditableTextComponent>(ecs::System*,
                                                                                                  [[maybe_unused]] const ecs::Id& entity,
                                                                                                  const ui::components::RectTransformComponent& parentRect,
                                                                                                  ui::components::RectTransformComponent& thisRect,
                                                                                                  const components::EditableTextComponent* text)
    {
        if (thisRect.rect.size.x == 0 && thisRect.rect.size.y == 0)
        {
            // need to calculate rect for children to calculate their desired size.
            thisRect.rect = util::CalculateScreenSpaceRect(thisRect, parentRect);
        }

        auto window = Application::Get()->GetPrimaryWindow();
        auto ret = ui::util::MeasureText(thisRect.rect,
                                         text->font,
                                         text->fontSize * window->GetContentScale(),
                                         text->inEditMode ? text->editText : text->text,
                                         true,
                                         text->wrap);
        return CalculateAnchorOffsets(thisRect, parentRect.rect) + ret;
    }
}
