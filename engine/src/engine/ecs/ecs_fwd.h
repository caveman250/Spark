#pragma once
#include "engine/bits/FlagUtil.h"

namespace se::ecs
{
    constexpr uint64_t s_InvalidEntity = 0;

    enum class IdFlags : uint32_t
    {
        None = 0
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

        bool operator != (const uint64_t& rhs) const
        {
            return id != rhs;
        }

        bool HasFlag(IdFlags flag) const
        {
            if (!flags)
            {
                return false;
            }

            return bits::GetFlag<IdFlags>(*flags, flag);
        }

        uint64_t id = 0;
        const std::string* name = nullptr;
        int32_t* flags = nullptr;
        const Id* scene = nullptr;
    };

    struct NullComponentType
    {
        static Id GetComponentId() { return s_InvalidEntity; }
    };

    bool IsEditorEntity(const Id& entity);
}

template <>
struct std::hash<se::ecs::Id>
{
    std::size_t operator()(const se::ecs::Id& rhs) const
    {
        return std::hash<uint64_t>()(rhs.id);
    }
};

template <>
struct std::formatter<se::ecs::Id> : std::formatter<std::string>
{
    template <class FormatContext>
    static auto format(const se::ecs::Id& obj, FormatContext& ctx)
    {
        return std::format_to(ctx.out(), "{}", obj.id);
    }
};
