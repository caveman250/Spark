#pragma once

#include "spark.h"
#include "engine/ecs/Signal.h"
#include "engine/reflect/Object.h"

namespace se::render::commands
{
    class RenderCommand;
}

namespace se::ui
{
    struct UILayerKey
    {
        UILayerKey(const int _layer, bool _editor = false) : layer(_layer), editor(_editor) {}
        int layer;
        bool editor;

        bool operator < (const UILayerKey& rhs) const
        {
            if (layer != rhs.layer)
            {
                return layer < rhs.layer;
            }

            return editor < rhs.editor;
        }
    };

    inline bool operator== (const UILayerKey& lhs, const UILayerKey& rhs)
    {
        return lhs.layer == rhs.layer && lhs.editor == rhs.editor;
    }

    struct UIRenderCommand
    {
        UIRenderCommand(render::commands::RenderCommand* _renderCmd, const UILayerKey& _layer = 0)
            : renderCmd(_renderCmd)
            , layer(_layer)
        {

        }
        render::commands::RenderCommand* renderCmd = nullptr;
        UILayerKey layer = 0;
    };
}

template <>
struct std::hash<se::ui::UILayerKey>
{
    std::size_t operator()(const se::ui::UILayerKey& rhs) const noexcept
    {
        size_t hash = 0;
        hash_combine(hash, rhs.layer, rhs.editor);
        return hash;
    }
};

namespace se::ui::singleton_components
{
    struct UIRenderComponent : ecs::Component
    {
        SPARK_SINGLETON_COMPONENT()

        std::unordered_map<ecs::Id, std::vector<UIRenderCommand>> entityPreRenderCommands = {};
        std::unordered_map<ecs::Id, std::vector<UIRenderCommand>> entityRenderCommands = {};
        std::unordered_map<ecs::Id, std::vector<UIRenderCommand>> entityPostRenderCommands = {};

        std::unordered_map<UILayerKey, size_t> LayerToRenderGroupMap = {};
    };
}
