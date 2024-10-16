#pragma once
#include "engine/string/String.h"

namespace se::ecs
{
#if SPARK_DIST
    typedef uint64_t Id;
#else
    struct Id
    {
        Id() = default;
        Id(uint64_t _id);
        Id(const Id& _id);
        Id(Id&& _id);

        operator uint64_t() const
        {
            return id;
        }

        Id& operator = (const Id& rhs);
        bool operator == (const Id& rhs) const
        {
            return id == rhs.id;
        }
        bool operator == (const uint64_t& rhs) const
        {
            return id == rhs;
        }

        uint64_t id = 0;
        const String* name = nullptr;
    };
#endif
}

#if !SPARK_DIST
template <>
struct std::hash<se::ecs::Id>
{
    std::size_t operator()(const se::ecs::Id& rhs) const
    {
        return std::hash<uint64_t>()(rhs.id);
    }
};
#endif
