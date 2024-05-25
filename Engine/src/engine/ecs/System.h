#pragma once

#include "Component.h"
#include "World.h"

namespace se::ecs
{
    typedef uint64_t SystemId;

    class BaseSystem
    {
    public:
        virtual ~BaseSystem() {}
        virtual void Update(float dt) = 0;

    protected:
        World* m_World = nullptr;

        friend class World;
    };

    template <typename... Cs>
    class System : public BaseSystem
    {
    public:
        System() {}

        void Update(float dt) override;

        static std::vector<std::pair<ComponentId, bool>> GetUsedComponents();

    private:
        virtual void OnUpdate(float dt, size_t count, Cs*... ts) = 0;

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index != sizeof...(Cs) + 1> UpdateBuilder(float dt, Ts... ts);

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index == sizeof...(Cs) + 1> UpdateBuilder(float dt, Ts... ts);
    };

    template <typename T>
    void CollectUsedComponent(std::vector<std::pair<ComponentId, bool>>& vec)
    {
        vec.push_back(std::make_pair(T::GetComponentId(), std::is_const<T>()));
    }

    template<typename... Cs>
    std::vector<std::pair<ComponentId, bool>> System<Cs...>::GetUsedComponents()
    {
        std::vector<std::pair<ComponentId, bool>> ret;
        (CollectUsedComponent<Cs>(ret), ...);
        return ret;
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::UpdateBuilder(float dt, Ts... ts)
    {
        using IthT = std::tuple_element<Index - 1, std::tuple<Cs...>>::type;

        UpdateBuilder<Index + 1>(dt,
                            ts...,
                            std::_Ph<Index + 2>());
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs) + 1> System<Cs...>::UpdateBuilder(float dt, Ts... ts)
    {
        m_World->Each<Cs...>(dt, std::bind(&System::OnUpdate, this, ts...));
    }

    template<typename... Cs>
    void System<Cs...>::Update(float dt)
    {
        UpdateBuilder<1>(dt, std::placeholders::_1, std::placeholders::_2);
    }
}
