#pragma once

#include "spark.h"
#include "engine/asset/font/Font.h"
#include "engine/asset/texture/Texture.h"
#include "engine/reflect/Reflect.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/IndexBuffer.h"
#include "engine/render/MaterialInstance.h"
#include "engine/string/String.h"
#include "engine/ui/Rect.h"
#include "engine/ui/util/MeshUtil.h"
#include "engine/ui/util/RectTransformUtil.h"
#include "engine/ui/text/Alignment.h"
#include "engine/ui/text/WrapMode.h"

namespace se::ui::systems
{
    class TextRenderSystem;
}

namespace se::ui::components
{
    struct TextComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_WIDGET_COMPONENT(TextComponent)

        std::shared_ptr<asset::Font> font = {};
        int fontSize = 0;
        String text = {};
        text::Alignment::Type alignment = text::Alignment::Left;
        text::WrapMode::Type wrap = text::WrapMode::Word;

        // internal use
        std::shared_ptr<render::MaterialInstance> materialInstance = {};
        std::shared_ptr<render::VertexBuffer> vertBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
        int lastFontSize = {};
        String lastText = {};
    };
}

#include "engine/ui/DesiredSizeCalculator.h"
namespace se::ui
{
    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::TextComponent>(ecs::System*,
                                                                                       [[maybe_unused]] const ecs::Id& entity,
                                                                                       const ui::components::RectTransformComponent& parentRect,
                                                                                       ui::components::RectTransformComponent& thisRect,
                                                                                       const components::TextComponent* text)
    {
        if (thisRect.rect.size.x == 0 && thisRect.rect.size.y == 0)
        {
            // need to calculate rect for children to calculate their desired size.
            thisRect.rect = util::CalculateScreenSpaceRect(thisRect, parentRect);
        }

        // treat char wrap as word wrap when it comes to desired size, or we will just get a single column string every time.
        auto wrap = text->wrap;
        if (wrap == text::WrapMode::Char)
        {
            wrap = text::WrapMode::Word;
        }
        auto ret = ui::util::MeasureText(thisRect.rect, text->font, text->fontSize, text->text, true, wrap);
        return ret;
    }
}
