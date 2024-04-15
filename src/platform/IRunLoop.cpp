#include "IRunLoop.h"

namespace se
{
    IRunLoop* IRunLoop::Get()
    {
        return s_Instance;
    }
}