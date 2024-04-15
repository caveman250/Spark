#pragma once

namespace se::math
{
    struct Vec3;
}

namespace se::render
{
    class VertexBuffer
    {
    public:
        static std::shared_ptr<VertexBuffer> CreateVertexBuffer(const std::vector<math::Vec3>& vertices);

        VertexBuffer(const std::vector<math::Vec3>& vertices);
        virtual void CreatePlatformResource() = 0;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
    protected:
        const std::vector<math::Vec3>& m_Vertices;
    };
}
