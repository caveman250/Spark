#pragma once

namespace se::mac::wrappers
{
    class RunLoop
    {
    public:
        static void Run();
        static void Shutdown();
    };
}
