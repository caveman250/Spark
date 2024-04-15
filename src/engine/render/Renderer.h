#pragma once
#include "RenderCommand.h"
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

        void SubmitRenderCommand(IWindow* window, const RenderCommand& renderCmd);

        virtual void Render(IWindow* window) = 0;
        virtual void EndFrame();
    protected:
        std::pmr::unordered_map<IWindow*, std::vector<RenderCommand>> RenderCommands;
    };
}
