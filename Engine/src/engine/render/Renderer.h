#pragma once

#include "engine/memory/Arena.h"
#include "RenderCommand.h"
#include "RenderState.h"
#include "spark.h"

namespace se
{
    class IWindow;
}

namespace se::render
{
    template<typename T>
    concept ARenderCommand = std::is_base_of<commands::RenderCommand, T>::value;

    class Renderer
    {
    public:
        static Renderer* Create();
        static void Shutdown();
        static Renderer* Get();

        virtual ~Renderer() {}
        virtual void Init() = 0;

        template <ARenderCommand T, typename... Args>
        void Submit(IWindow* window, Args&&... args);
        void ApplyRenderState(const RenderState& rs);

        virtual void Render(IWindow* window) = 0;
        virtual void EndFrame();
    protected:
        void ExecuteDrawCommands(IWindow* window);
        virtual void ApplyDepthCompare(DepthCompare comp) = 0;

        std::unordered_map<IWindow*, std::vector<commands::RenderCommand*>> m_RenderCommands;
        RenderState m_CachedRenderState;
        memory::Arena m_RenderCommandsArena;
    };

    template<ARenderCommand T, typename... Args>
    void Renderer::Submit(IWindow* window, Args&&... args)
    {
        m_RenderCommands[window].push_back(m_RenderCommandsArena.Alloc<T>(std::forward<Args>(args)...));
    }
}
