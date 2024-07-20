#include "Mipmap.h"

namespace se::asset::texture
{
    DEFINE_SPARK_CLASS_BEGIN(Mipmap)
        DEFINE_MEMBER(m_Data)
        DEFINE_MEMBER(sizeX)
        DEFINE_MEMBER(sizeY)
    DEFINE_SPARK_CLASS_END()
}