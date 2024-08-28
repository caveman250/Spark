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
        void DoAction(Id entity, World* world, const std::function<void(Cs*...)>& func);

        template<size_t Index, typename... Ts>
        std::enable_if_t<Index == sizeof...(Cs)> CallWrappedFunc(Id entity, World *world,
                                                                 const std::function<void(Cs *...)> &func, Ts... ts);

        template<size_t Index, typename... Ts>
        std::enable_if_t<Index != sizeof...(Cs)> CallWrappedFunc(Id entity, World *world,
                                                                 const std::function<void(Cs *...)> &func, Ts... ts);
    };

    template<typename... Cs>
    template<size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs)> SignalActionBuilder<Cs...>::CallWrappedFunc(Id, World*, const std::function<void(Cs*...)>& func, Ts... ts)
    {
        func(ts...);
    }

    template<typename... Cs>
    template<size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs)> SignalActionBuilder<Cs...>::CallWrappedFunc(Id entity, World* world, const std::function<void(Cs*...)>& func, Ts... ts)
    {
        using IthT = std::tuple_element<Index, std::tuple<Cs...>>::type;

        auto comp = world->GetComponent<IthT>(entity);
        CallWrappedFunc<Index + 1>(entity, world, func, ts..., comp);
    }


    template<typename... Cs>
    void SignalActionBuilder<Cs...>::DoAction(Id entity, World* world, const std::function<void(Cs*...)>& func)
    {
        CallWrappedFunc<0>(entity, world, func);
    }
}
