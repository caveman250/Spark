#include "VertexBuffer.h"

namespace se::render
{
    VertexBuffer::VertexBuffer(const std::vector<VertexStream>& streams)
    {
        for (const auto& stream : streams)
        {
            if (SPARK_VERIFY(!m_VertexStreams.contains(stream.type)))
            {
                m_VertexStreams[stream.type] = stream;
            }
        }
    }
}
