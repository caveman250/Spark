#pragma once

#include "spark.h"
#include "engine/asset/font/Font.h"
#include "engine/asset/texture/Texture.h"
#include "engine/reflect/Reflect.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/IndexBuffer.h"
#include "engine/render/Material.h"
#include "engine/string/String.h"
#include "engine/ui/Rect.h"

namespace se::ui::systems
{
    class TextRenderSystem;
}

namespace se::ui::components
{
    struct TextComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(TextComponent)

        std::shared_ptr<asset::Font> font = {};
        int fontSize = 0;
        String text = {};

    private:
        // internal use
        std::shared_ptr<render::Material> material = {};
        std::shared_ptr<render::VertexBuffer> vertBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
        Rect lastRect = {};
        int lastFontSize = {};
        String lastText = {};

        friend class systems::TextRenderSystem;
    };
}
