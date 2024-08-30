#pragma once

#include "spark.h"
#include "Archetype.h"
#include "World.h"

namespace se::ecs
{
    template<typename... Cs>
    class SignalActionBuilder
    {
    public:
        template <typename... As, typename Func>
        void DoAction(Id entity, World* world, const Func& func, As... args);

        template<size_t Index, typename... Ts, typename Func>
        std::enable_if_t<Index == sizeof...(Cs)> CallWrappedFunc(Id entity, World *world,
                                                                 const Func& func, Ts... ts);

        template<size_t Index, typename... Ts, typename Func>
        std::enable_if_t<Index != sizeof...(Cs)> CallWrappedFunc(Id entity, World *world,
                                                                 const Func& func, Ts... ts);
    };

    template<typename... Cs>
    template<size_t Index, typename... Ts, typename Func>
    std::enable_if_t<Index == sizeof...(Cs)> SignalActionBuilder<Cs...>::CallWrappedFunc(Id, World*, const Func& func, Ts... ts)
    {
        func(ts...);
    }

    template<typename... Cs>
    template<size_t Index, typename... Ts, typename Func>
    std::enable_if_t<Index != sizeof...(Cs)> SignalActionBuilder<Cs...>::CallWrappedFunc(Id entity, World* world, const Func& func, Ts... ts)
    {
        using IthT = std::tuple_element<Index, std::tuple<Cs...>>::type;

        auto comp = world->GetComponent<IthT>(entity);
        CallWrappedFunc<Index + 1>(entity, world, func, ts..., comp);
    }


    template<typename... Cs>
    template <typename... As, typename Func>
    void SignalActionBuilder<Cs...>::DoAction(Id entity, World* world, const Func& func, As... args)
    {
        CallWrappedFunc<0>(entity, world, func, args...);
    }
}
