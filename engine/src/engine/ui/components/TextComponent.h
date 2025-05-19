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
#include "platform/IWindow.h"

namespace se::ui::systems
{
    class TextRenderSystem;
}

namespace se::ui::components
{
    struct TextComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        std::shared_ptr<asset::Font> font = {};

        SPARK_MEMBER(Serialized)
        int fontSize = 0;

        SPARK_MEMBER(Serialized)
        String text = {};

        SPARK_MEMBER(Serialized)
        text::Alignment alignment = text::Alignment::Left;

        SPARK_MEMBER(Serialized)
        text::WrapMode wrap = text::WrapMode::Word;

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

        auto window = Application::Get()->GetPrimaryWindow();
        auto ret = ui::util::MeasureText(thisRect.rect, text->font, static_cast<int>(text->fontSize * window->GetContentScale()), text->text, true, text->wrap);
        return CalculateAnchorOffsets(thisRect, parentRect.rect) + ret;
    }
}
