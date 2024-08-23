#pragma once
#include "engine/memory/BinaryBlob.h"

namespace se::asset::builder::util
{
    static constexpr int s_BitmapHeaderSize = 54;
    memory::BinaryBlob CreateBitmapData(int width, int height);
}
