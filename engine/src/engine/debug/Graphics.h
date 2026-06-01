#pragma once
#include "engine/math/math.h"

namespace se::render
{
    class MaterialInstance;
}

namespace se::debug
{
    struct Line
    {
        math::Vec3 start = {};
        math::Vec3 end = {};
        math::Vec4 colour = {};
    };

    class Graphics
    {
    public:
        static Graphics& Get();

        void Render();

        void AddLine(const math::Vec3& start, const math::Vec3& end, const math::Vec4& colour);

    private:

        std::shared_ptr<render::MaterialInstance> m_LineMaterial;
        std::vector<Line> m_Lines = {};
    };
}
