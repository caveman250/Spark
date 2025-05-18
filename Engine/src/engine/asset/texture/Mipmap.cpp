#include "Mipmap.h"

namespace se::asset::texture
{
    Mipmap::Mipmap() {};
    Mipmap::Mipmap(memory::BinaryBlob data, uint32_t sizeX, uint32_t sizeY)
        : m_Data(data)
        , m_SizeX(sizeX)
        , m_SizeY(sizeY)
    {}
}