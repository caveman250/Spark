#include "VertexBuffer.h"

namespace se::render
{
    VertexBuffer::VertexBuffer(const std::vector<math::Vec3> &vertices)
        : m_Vertices(vertices)
    {
    }
}
