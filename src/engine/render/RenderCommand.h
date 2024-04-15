#pragma once
#include "spark.h"

namespace se
{
    class IWindow;
}

namespace se::render
{
    struct RenderCommand
    {
        RenderCommand(const std::function<void()>& cmd) : command(cmd) {}
        std::function<void()> command;
    };
}
