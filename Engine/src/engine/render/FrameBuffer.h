#pragma once

#include "spark.h"

namespace se::render
{
    class FrameBuffer
    {
    public:
        virtual ~FrameBuffer() = default;
        static std::shared_ptr<FrameBuffer> CreateFrameBuffer();
        virtual void OnPushed() = 0;
        virtual void OnPopped() = 0;
    };
}
