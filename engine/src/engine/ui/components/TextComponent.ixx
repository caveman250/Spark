module;

#include "spark.h"


#include "engine/ui/text/Alignment.h"
#include "engine/ui/text/WrapMode.h"
#include "engine/ui/util/MeshUtil.h"
#include "engine/ui/util/RectTransformUtil.h"
#include "platform/IWindow.h"

export module TextComponent;
import DesiredSizeCalculator;
import System;
import AssetReference;
import Font;
import MaterialInstance;

namespace se::ui::components
{
    export struct TextComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        SPARK_MEMBER(Serialized)
        asset::AssetReference<asset::Font> font = {};

        SPARK_MEMBER(Serialized)
        int fontSize = 0;

        SPARK_MEMBER(Serialized)
        std::string text = {};

        SPARK_MEMBER(Serialized)
        math::Vec3 textColour = { 1.f };

        SPARK_MEMBER(Serialized)
        text::Alignment alignment = text::Alignment::Left;

        SPARK_MEMBER(Serialized)
        text::WrapMode wrap = text::WrapMode::Word;

        // internal use
        std::shared_ptr<render::MaterialInstance> materialInstance = {};
        std::shared_ptr<render::VertexBuffer> vertBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
        int lastFontSize = {};
        std::string lastText = {};
    };
}

namespace se::ui
{
    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::TextComponent>(ecs::System*,
                                                                                       [[maybe_unused]] const ecs::Id& entity,
                                                                                       components::RectTransformComponent& thisRect,
                                                                                       const components::TextComponent* text)
    {
        const auto window = Application::Get()->GetWindow();
        const auto ret = util::MeasureText(&thisRect.rect,
            text->font.GetAsset().get(),
            static_cast<int>(text->fontSize * window->GetContentScale()),
            &text->text,
            true,
            text->wrap);
        return ret;
    }
}
