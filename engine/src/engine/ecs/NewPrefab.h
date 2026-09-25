#pragma once

#include "spark.h"
#include "ecs_fwd.h"

namespace se::ecs
{
    template <typename... Cs>
    class NewPrefabInvoker
    {
    public:
        template<size_t... Is>
        static void CallFunc(const Id& entity, const std::function<void(const Id&, Cs*...)>& func, std::array<void*, sizeof...(Cs)>& components, std::index_sequence<Is...>)
        {
            func(entity, static_cast<Cs*>(components[Is])...);
        }
    };

    class NewPrefab
    {
    public:
        const Id& GetEntity() const { return entity; }

        template <typename... Ts>
        void ForEachEntity(const std::function<void(const Id&, Ts*...)>& func)
        {
            for (const auto& kvp : components)
            {
                bool hasComp = true;
                int index = 0;
                std::array<void*, sizeof...(Ts)> components = { nullptr };
                (HasComp<Ts, Ts...>(kvp.second, components, hasComp, index), ...);
                if (hasComp)
                {
                    NewPrefabInvoker<Ts...>::CallFunc(kvp.first, func, components, std::make_index_sequence<sizeof...(Ts)>{});
                }
            }
        }

    private:
        template <typename T, typename... Ts>
        static void HasComp(const std::unordered_map<Id, void*>& entity, std::array<void*, sizeof...(Ts)>& components, bool& hasComp, int& index)
        {
            auto it = entity.find(T::GetComponentId());
            if (it == entity.end())
            {
                hasComp = false;
                return;
            }
            components[index] = it->second;
            hasComp &= true;
            index++;
        }

        void AddComponent(const Id& entity, const Id& componentType, void* component)
        {
            components[entity].insert(std::make_pair(componentType, component));
        }

        std::unordered_map<Id, std::unordered_map<Id, void*>> components;
        Id entity;

        friend class World;
    };
}
