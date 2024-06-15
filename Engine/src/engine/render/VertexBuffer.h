#pragma once

namespace se::math
{
    struct Vec3;
}

namespace se::render
{
    enum class VertexStreamType
    {
        Position,
        Colour,
        UV,
        Normal,
        Tangent,
        Bitangent
    };
    struct VertexStream
    {
        VertexStreamType type;
        uint8_t stride;
        std::vector<float> data;
    };

    class VertexBuffer
    {
    public:
        static std::shared_ptr<VertexBuffer> CreateVertexBuffer(const std::vector<VertexStream>& streams);

        VertexBuffer(const std::vector<VertexStream>& streams);
        virtual ~VertexBuffer() = default;
        virtual void CreatePlatformResource() = 0;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
    protected:
        std::map<VertexStreamType, VertexStream> m_VertexStreams;
    };
}
