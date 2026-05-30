#include "MeshUtil.h"

namespace se::geo::util
{
    asset::StaticMesh CreateRectMesh(const math::Vec2& topLeft, const math::Vec2& size)
    {
        asset::StaticMesh mesh;
        mesh.vertices =
        {
            { topLeft.x, size.y, 0 },
            { size.x, size.y, 0 },
            { size.x, topLeft.y, 0 },
            { topLeft.x, topLeft.y, 0 },
        };
        mesh.indices = { 1, 3, 0, 3, 1, 2 };
        mesh.uvs =
        {
                { 0, 1 },
                { 1, 1 },
                { 1, 0 },
                { 0, 0 }
        };
        mesh.normals =
        {
            { 0.f, 0.f, 1.0f },
            { 0.f, 0.f, 1.0f },
            { 0.f, 0.f, 1.0f },
            { 0.f, 0.f, 1.0f },
        };
        return mesh;
    }

    asset::StaticMesh CreateCircleMesh(float radius, size_t steps, size_t startIndex, size_t endIndex)
    {
        asset::StaticMesh mesh;
        mesh.vertices.push_back(math::Vec3(0.0f, 0.0f, 0.0f));
        mesh.uvs.push_back(math::Vec2(0.5f, 0.5f));

        if (endIndex == std::numeric_limits<size_t>::max() || endIndex > steps || endIndex < startIndex)
        {
            endIndex = steps;
        }

        math::Vec3 min = std::numeric_limits<float>::min(), max = std::numeric_limits<float>::max();
        for (size_t i = startIndex; i <= endIndex; ++i)
        {
            float angle = static_cast<float>(2.0f * M_PI * i / steps);
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            mesh.vertices.push_back(math::Vec3(x, y, 0.0f));
            mesh.uvs.push_back(math::Vec2(0.5f + x / (2.0f * radius), 0.5f + y / (2.0f * radius)));
            mesh.normals.push_back(math::Vec3(0.f, 0.f, 1.0f));

            min = math::Min(min, mesh.vertices.back());
            max = math::Max(max, mesh.vertices.back());
        }

        for (size_t i = 1; i <= steps; ++i)
        {
            mesh.indices.push_back(0);
            mesh.indices.push_back(static_cast<int>(i));
            mesh.indices.push_back(static_cast<int>(i + 1));
        }

        mesh.aabb = geo::AABB
        {
            .pos = min,
            .size = max - min
        };

        return mesh;
    }

    asset::StaticMesh CreateCircleMesh(float radius, float innerRadius, size_t steps, size_t startIndex, size_t endIndex)
    {
        asset::StaticMesh mesh;
        if (endIndex == std::numeric_limits<size_t>::max() || endIndex > steps || endIndex < startIndex)
        {
            endIndex = steps;
        }

        math::Vec3 min = std::numeric_limits<float>::max();
        math::Vec3 max = std::numeric_limits<float>::lowest();

        for (size_t i = startIndex; i <= endIndex; ++i)
        {
            float angle = static_cast<float>(2.0f * M_PI * i / steps);

            float cosA = cos(angle);
            float sinA = sin(angle);

            const float outerX = radius * cosA;
            const float outerY = radius * sinA;
            mesh.vertices.push_back(math::Vec3(outerX, outerY, 0.0f));
            mesh.uvs.push_back(math::Vec2(0.5f + outerX / (2.0f * radius), 0.5f + outerY / (2.0f * radius)));
            mesh.normals.push_back(math::Vec3(0.f, 0.f, 1.0f));

            const float innerX = innerRadius * cosA;
            const float innerY = innerRadius * sinA;
            mesh.vertices.push_back(math::Vec3(innerX, innerY, 0.0f));
            mesh.uvs.push_back(math::Vec2(0.5f + innerX / (2.0f * radius), 0.5f + innerY / (2.0f * radius)));
            mesh.normals.push_back(math::Vec3(0.f, 0.f, 1.0f));

            min = math::Min(min, mesh.vertices[mesh.vertices.size() - 2]);
            min = math::Min(min, mesh.vertices.back());
            max = math::Max(max, mesh.vertices[mesh.vertices.size() - 2]);
            max = math::Max(max, mesh.vertices.back());
        }

        for (size_t i = 0; i < (endIndex - startIndex); ++i)
        {
            const int outer0 = static_cast<int>(i * 2);
            const int inner0 = static_cast<int>(i * 2 + 1);
            const int outer1 = static_cast<int>((i + 1) * 2);
            const int inner1 = static_cast<int>((i + 1) * 2 + 1);

            mesh.indices.push_back(outer0);
            mesh.indices.push_back(inner0);
            mesh.indices.push_back(outer1);

            mesh.indices.push_back(outer1);
            mesh.indices.push_back(inner0);
            mesh.indices.push_back(inner1);
        }

        mesh.aabb = geo::AABB
        {
            .pos = min,
            .size = max - min + math::Vec3(0.f, 0.f, 0.1f)
        };

        return mesh;
    }
}