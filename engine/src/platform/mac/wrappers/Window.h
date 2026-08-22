#pragma once
#include "engine/render/metal/MTL_fwd.h"

namespace se::mac::wrappers
{
    class Window
    {
    public:
        static NSWindowPtr CreateWindow(int x, int y);
        static SparkViewPtr CreateView(int x, int y, MTLDevicePtr device);
        static float InitContentScale(SparkViewPtr view);
        static void AssignView(NSWindowPtr window, SparkViewPtr view);
        static void Cleanup(NSWindowPtr window);
    };
}
