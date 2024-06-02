#pragma once

#include "spark.h"
#include "Archetype.h"

namespace se::ecs
{
    template<typename... Cs>
    class Action
    {
    public:
        template<typename Func>
        static void DoAction(const std::vector<EntityId>& entities, const std::unordered_map<ComponentId, void*>& singletonCompData, Archetype* archetype, Func&& func);

        template<std::size_t Index, typename Func, typename... Ts>
        static std::enable_if_t<Index != sizeof...(Cs)> ActionBuilder(const std::vector<EntityId>& entities,
                                                                      const std::unordered_map<ComponentId, void*>& singletonCompData,
                                                                      const Type& archetypeType,
                                                                      const std::vector<ComponentList>& compData,
                                                                      Func&& func,
                                                                      Ts... ts);

        template<std::size_t Index, typename Func, typename... Ts>
        static std::enable_if_t<Index == sizeof...(Cs)> ActionBuilder(const std::vector<EntityId>& entities,
                                                                      const std::unordered_map<ComponentId, void*>& singletonCompData,
                                                                      const Type& archetypeType,
                                                                      const std::vector<ComponentList>& compData,
                                                                      Func&& func,
                                                                      Ts... ts);
    };

    template<typename... Cs>
    template<std::size_t Index, typename Func, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs)> Action<Cs...>::ActionBuilder(const std::vector<EntityId>& entities,
                                                                          const std::unordered_map<ComponentId, void*>&,
                                                                          const Type&,
                                                                          const std::vector<ComponentList>&,
                                                                          Func&& func,
                                                                          Ts... ts)
    {
        func(entities, ts...);
    }

    template<typename... Cs>
    template<std::size_t Index, typename Func, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs)> Action<Cs...>::ActionBuilder(const std::vector<EntityId>& entities,
                                                                          const std::unordered_map<ComponentId, void*>& singletonCompData,
                                                                          const Type& archetypeType,
                                                                          const std::vector<ComponentList>& compData,
                                                                          Func&& func,
                                                                          Ts... ts)
    {
        using IthT = std::tuple_element<Index, std::tuple<Cs...>>::type;

        IthT* argData = nullptr;
        if (IthT::IsSingletonComponent())
        {
            SPARK_ASSERT(singletonCompData.contains(IthT::GetComponentId()));
            argData = reinterpret_cast<IthT*>(singletonCompData.at(IthT::GetComponentId()));
        }
        else
        {
            std::size_t compIndex = 0;
            ComponentId thisTypeCS = IthT::s_StaticId;
            ComponentId thisArchetypeID = archetypeType[compIndex];
            while (thisTypeCS != thisArchetypeID && compIndex < archetypeType.size())
            {
                ++compIndex;
                thisArchetypeID = archetypeType[compIndex];
            }

            if (compIndex == archetypeType.size())
            {
                debug::Log::Fatal("System was executed against an incorrect Archetype");
            }

            argData = reinterpret_cast<IthT*>(compData[compIndex].Data());
        }

        ActionBuilder<Index + 1>(entities,
                                 singletonCompData,
                                 archetypeType,
                                 compData,
                                 func,
                                 ts...,
                                 argData);
    }


    template<typename... Cs>
    template<typename Func>
    void Action<Cs...>::DoAction(const std::vector<EntityId>& entities, const std::unordered_map<ComponentId, void*>& singletonCompData, Archetype* archetype, Func&& func)
    {
        ActionBuilder<0>(entities,
                         singletonCompData,
                         archetype->type,
                         archetype->components,
                         func);
    }
}
