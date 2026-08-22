module;

#include <mutex>

export module Spark.Debug.Graphics;
import MaterialInstance;
import Spark.Math;

namespace se::debug
{
    export struct Line
    {
        math::Vec3 start = {};
        math::Vec3 end = {};
        math::Vec4 colour = {};
    };

    export class Graphics
    {
    public:
        static Graphics& Get();

        void Render();

        void AddLine(const math::Vec3& start, const math::Vec3& end, const math::Vec4& colour);

    private:
        std::mutex m_LinesMutex = {};
        std::shared_ptr<render::MaterialInstance> m_LineMaterial;
        std::vector<Line> m_Lines = {};
    };
}
