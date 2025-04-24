#pragma once

#include "spark.h"

namespace se::render
{
    class FrameBuffer
    {
    public:
        virtual ~FrameBuffer() = default;
        static std::shared_ptr<FrameBuffer> CreateFrameBuffer();
        virtual void Bind() = 0;
        virtual void Commit() = 0;
    };
}
