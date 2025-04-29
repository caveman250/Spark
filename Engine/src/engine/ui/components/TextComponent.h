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
        text::Alignment::Type justification = text::Alignment::Left;

        // internal use
        std::shared_ptr<render::MaterialInstance> materialInstance = {};
        std::shared_ptr<render::VertexBuffer> vertBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
        Rect lastRect = {};
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
                                                                                       const ui::components::RectTransformComponent& thisRect,
                                                                                       const components::TextComponent* text)
    {
        Rect bounds = util::CalculateScreenSpaceRect(thisRect, parentRect);
        auto ret = ui::util::MeasureText(bounds, text->font, text->fontSize, text->text, true, true);
        return ret;
    }
}
