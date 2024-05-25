#pragma once

namespace se::ecs
{
    typedef uint64_t ComponentId;

    template<class _Alloc>
    struct std::hash<std::vector<se::ecs::ComponentId, _Alloc>>
    {
        std::size_t operator()(const std::vector<se::ecs::ComponentId, _Alloc>& vec) const
        {
            std::size_t seed = vec.size();
            for (auto x: vec)
            {
                x = ((x >> 16) ^ x) * 0x45d9f3b;
                x = ((x >> 16) ^ x) * 0x45d9f3b;
                x = (x >> 16) ^ x;
                seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}