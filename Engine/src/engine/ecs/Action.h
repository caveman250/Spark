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
        static void DoAction(float dt, Archetype* archetype, Func&& func);

        template<std::size_t Index, typename Func, typename... Ts>
        static std::enable_if_t<Index != sizeof...(Cs)> ActionBuilder(float dt,
                                                               const Type& archetypeType,
                                                               const std::vector<ComponentList>& compData,
                                                               Func&& func,
                                                               Ts... ts);

        template<std::size_t Index, typename Func, typename... Ts>
        static std::enable_if_t<Index == sizeof...(Cs)> ActionBuilder(float dt,
                                                               const Type& archetypeType,
                                                               const std::vector<ComponentList>& compData,
                                                               Func&& func,
                                                               Ts... ts);
    };

    template<typename... Cs>
    template<std::size_t Index, typename Func, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs)> Action<Cs...>::ActionBuilder(float dt,
                                                                          const Type&,
                                                                          const std::vector<ComponentList>& compData,
                                                                          Func&& func,
                                                                          Ts... ts)
    {
        func(dt, compData[0].Count(), ts...);
    }

    template<typename... Cs>
    template<std::size_t Index, typename Func, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs)> Action<Cs...>::ActionBuilder(float dt,
                                                                          const Type& archetypeType,
                                                                          const std::vector<ComponentList>& compData,
                                                                          Func&& func,
                                                                          Ts... ts)
    {
        using IthT = std::tuple_element<Index, std::tuple<Cs...>>::type;
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
            throw std::runtime_error("System was executed against an incorrect Archetype"); //TODO
        }

        ActionBuilder<Index + 1>(dt,
                            archetypeType,
                            compData,
                            func,
                            ts...,
                            reinterpret_cast<IthT*>(compData[compIndex].Data()));
    }


    template<typename... Cs>
    template<typename Func>
    void Action<Cs...>::DoAction(float dt, Archetype* archetype, Func&& func)
    {
        ActionBuilder<0>(dt,
                    archetype->type,
                    archetype->components,
                    func);
    }
}
