#pragma once
#include "engine/string/String.h"

namespace se::ecs
{
    constexpr uint64_t s_InvalidEntity = 0;

    enum class IdFlags : uint32_t
    {
        None = 0,
        Editor = 1,
    };

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
        int32_t* flags = nullptr;
    };

    struct NullComponentType
    {
        static se::ecs::Id GetComponentId() { return se::ecs::s_InvalidEntity; }
    };
}

template <>
struct std::hash<se::ecs::Id>
{
    std::size_t operator()(const se::ecs::Id& rhs) const
    {
        return std::hash<uint64_t>()(rhs.id);
    }
};
