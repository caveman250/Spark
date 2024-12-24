#include "Mipmap.h"

namespace se::asset::texture
{
    DEFINE_SPARK_CLASS_BEGIN(Mipmap)
        DEFINE_SERIALIZED_MEMBER(m_Data)
        DEFINE_SERIALIZED_MEMBER(m_SizeX)
        DEFINE_SERIALIZED_MEMBER(m_SizeY)
    DEFINE_SPARK_CLASS_END()

    Mipmap::Mipmap() {};
    Mipmap::Mipmap(memory::BinaryBlob data, uint32_t sizeX, uint32_t sizeY)
        : m_Data(data)
        , m_SizeX(sizeX)
        , m_SizeY(sizeY)
    {}
}