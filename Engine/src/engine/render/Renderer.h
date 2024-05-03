#pragma once
#include "RenderCommand.h"
#include "RenderState.h"
#include "spark.h"

namespace se
{
    class IWindow;
}

namespace se::render
{
    class Renderer
    {
    public:
        static Renderer* Create();
        static void Shutdown();
        static Renderer* Get();

        virtual ~Renderer() {}
        virtual void Init() = 0;

        void Submit(IWindow* window, const RenderCommand& renderCmd);
        void ApplyRenderState(const RenderState& rs);

        virtual void Render(IWindow* window) = 0;
        virtual void EndFrame();
    protected:
        void ExecuteDrawCommands(IWindow* window);
        virtual void ApplyDepthCompare(DepthCompare comp) = 0;

        std::pmr::unordered_map<IWindow*, std::vector<RenderCommand>> RenderCommands;
        RenderState m_CachedRenderState;
    };
}
