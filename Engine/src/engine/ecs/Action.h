#pragma once

#include <engine/reflect/Object.h>

#include "spark.h"
#include "Archetype.h"

namespace se::ecs
{
    template<typename... Cs>
    class Action
    {
    public:
        template<typename Func>
        static void DoAction(const std::vector<Id>& entities, const std::unordered_map<Id, reflect::ObjectBase*>& singletonCompData, Archetype* archetype, Func&& func);

        template<std::size_t Index, typename Func, typename... Ts>
        static std::enable_if_t<Index != sizeof...(Cs)> ActionBuilder(const std::vector<Id>& entities,
                                                                      const std::unordered_map<Id, reflect::ObjectBase*>& singletonCompData,
                                                                      const Type& archetypeType,
                                                                      const std::vector<ComponentList>& compData,
                                                                      Func&& func,
                                                                      Ts... ts);

        template<std::size_t Index, typename Func, typename... Ts>
        static std::enable_if_t<Index == sizeof...(Cs)> ActionBuilder(const std::vector<Id>& entities,
                                                                      const std::unordered_map<Id, reflect::ObjectBase*>& singletonCompData,
                                                                      const Type& archetypeType,
                                                                      const std::vector<ComponentList>& compData,
                                                                      Func&& func,
                                                                      Ts... ts);

        template<typename Func>
        static bool DoBoolAction(const std::vector<Id>& entities, const std::unordered_map<Id, reflect::ObjectBase*>& singletonCompData, Archetype* archetype, Func&& func);

        template<std::size_t Index, typename Func, typename... Ts>
        static std::enable_if_t<Index != sizeof...(Cs), bool> BoolActionBuilder(const std::vector<Id>& entities,
                                                                      const std::unordered_map<Id, reflect::ObjectBase*>& singletonCompData,
                                                                      const Type& archetypeType,
                                                                      const std::vector<ComponentList>& compData,
                                                                      Func&& func,
                                                                      Ts... ts);

        template<std::size_t Index, typename Func, typename... Ts>
        static std::enable_if_t<Index == sizeof...(Cs), bool> BoolActionBuilder(const std::vector<Id>& entities,
                                                                      const std::unordered_map<Id, reflect::ObjectBase*>& singletonCompData,
                                                                      const Type& archetypeType,
                                                                      const std::vector<ComponentList>& compData,
                                                                      Func&& func,
                                                                      Ts... ts);
    };

    template<typename... Cs>
    template<std::size_t Index, typename Func, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs)> Action<Cs...>::ActionBuilder(const std::vector<Id>& entities,
                                                                          const std::unordered_map<Id, reflect::ObjectBase*>&,
                                                                          const Type&,
                                                                          const std::vector<ComponentList>&,
                                                                          Func&& func,
                                                                          Ts... ts)
    {
        func(entities, ts...);
    }

    template<typename ... Cs>
    template<typename Func>
    bool Action<Cs...>::DoBoolAction(const std::vector<Id>& entities,
        const std::unordered_map<Id, reflect::ObjectBase*>& singletonCompData, Archetype* archetype, Func&& func)
    {
        return BoolActionBuilder<0>(entities,
                         singletonCompData,
                         archetype ? archetype->type : Type(),
                         archetype ? archetype->components : std::vector<ComponentList>(),
                         func);
    }

    template<typename ... Cs>
    template<std::size_t Index, typename Func, typename ... Ts>
    std::enable_if_t<Index != sizeof...(Cs), bool> Action<Cs...>::BoolActionBuilder(const std::vector<Id>& entities,
        const std::unordered_map<Id, reflect::ObjectBase*>& singletonCompData, const Type& archetypeType,
        const std::vector<ComponentList>& compData, Func&& func, Ts... ts)
    {
        using IthT = std::tuple_element<Index, std::tuple<Cs...>>::type;

        if (IthT::IsSingletonComponent())
        {
            IthT* data = nullptr;
            if (singletonCompData.contains(IthT::GetComponentId()))
                data = reinterpret_cast<IthT*>(singletonCompData.at(IthT::GetComponentId()));
            return BoolActionBuilder<Index + 1>(entities,
                                     singletonCompData,
                                     archetypeType,
                                     compData,
                                     func,
                                     ts...,
                                     data);
        }
        else if (!archetypeType.empty())
        {
            std::size_t compIndex = 0;
            Id thisTypeCS = IthT::GetComponentId();
            Id thisArchetypeID = archetypeType[compIndex];
            while (thisTypeCS != thisArchetypeID && compIndex < archetypeType.size())
            {
                ++compIndex;
                thisArchetypeID = archetypeType[compIndex];
            }

            if (compIndex == archetypeType.size())
            {
                debug::Log::Fatal("System was executed against an incorrect Archetype");
            }

            return BoolActionBuilder<Index + 1>(entities,
                                     singletonCompData,
                                     archetypeType,
                                     compData,
                                     func,
                                     ts...,
                                     reinterpret_cast<IthT*>(compData[compIndex].Data()));
        }
        else
        {
            return BoolActionBuilder<Index + 1>(entities,
                                     singletonCompData,
                                     archetypeType,
                                     compData,
                                     func,
                                     ts...,
                                     nullptr);
        }
    }

    template<typename ... Cs>
    template<std::size_t Index, typename Func, typename ... Ts>
    std::enable_if_t<Index == sizeof...(Cs), bool> Action<Cs...>::BoolActionBuilder(const std::vector<Id>& entities,
        const std::unordered_map<Id, reflect::ObjectBase*>&, const Type&,
        const std::vector<ComponentList>&, Func&& func, Ts... ts)
    {
        return func(entities, ts...);
    }

    template<typename... Cs>
    template<std::size_t Index, typename Func, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs)> Action<Cs...>::ActionBuilder(const std::vector<Id>& entities,
                                                                          const std::unordered_map<Id, reflect::ObjectBase*>& singletonCompData,
                                                                          const Type& archetypeType,
                                                                          const std::vector<ComponentList>& compData,
                                                                          Func&& func,
                                                                          Ts... ts)
    {
        using IthT = std::tuple_element<Index, std::tuple<Cs...>>::type;

        if (IthT::IsSingletonComponent())
        {
            IthT* data = nullptr;
            if (singletonCompData.contains(IthT::GetComponentId()))
                data = reinterpret_cast<IthT*>(singletonCompData.at(IthT::GetComponentId()));
            ActionBuilder<Index + 1>(entities,
                                     singletonCompData,
                                     archetypeType,
                                     compData,
                                     func,
                                     ts...,
                                     data);
        }
        else if (!archetypeType.empty())
        {
            std::size_t compIndex = 0;
            Id thisTypeCS = IthT::GetComponentId();
            Id thisArchetypeID = archetypeType[compIndex];
            while (thisTypeCS != thisArchetypeID && compIndex < archetypeType.size())
            {
                ++compIndex;
                thisArchetypeID = archetypeType[compIndex];
            }

            if (compIndex == archetypeType.size())
            {
                debug::Log::Fatal("System was executed against an incorrect Archetype");
            }

            ActionBuilder<Index + 1>(entities,
                                     singletonCompData,
                                     archetypeType,
                                     compData,
                                     func,
                                     ts...,
                                     reinterpret_cast<IthT*>(compData[compIndex].Data()));
        }
        else
        {
            ActionBuilder<Index + 1>(entities,
                                     singletonCompData,
                                     archetypeType,
                                     compData,
                                     func,
                                     ts...,
                                     nullptr);
        }
    }


    template<typename... Cs>
    template<typename Func>
    void Action<Cs...>::DoAction(const std::vector<Id>& entities, const std::unordered_map<Id, reflect::ObjectBase*>& singletonCompData, Archetype* archetype, Func&& func)
    {
        ActionBuilder<0>(entities,
                         singletonCompData,
                         archetype ? archetype->type : Type(),
                         archetype ? archetype->components : std::vector<ComponentList>(),
                         func);
    }
}
