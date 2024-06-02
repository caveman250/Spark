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
        virtual void Update() = 0;

    protected:
        World* m_World = nullptr;

        friend class World;
    };

    template <typename... Cs>
    class System : public BaseSystem
    {
    public:
        System() {}

        void Update() override;

        static std::vector<std::pair<ComponentId, bool>> GetUsedComponents();

    private:
        virtual void OnUpdate(const std::vector<se::ecs::EntityId>& entities, Cs*... ts) = 0;

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index != sizeof...(Cs) + 1> UpdateBuilder(Ts... ts);

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index == sizeof...(Cs) + 1> UpdateBuilder(Ts... ts);
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
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::UpdateBuilder(Ts... ts)
    {
        using IthT = std::tuple_element<Index - 1, std::tuple<Cs...>>::type;

        UpdateBuilder<Index + 1>(ts..., std::_Ph<Index + 1>());
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs) + 1> System<Cs...>::UpdateBuilder(Ts... ts)
    {
        m_World->Each<Cs...>(std::bind(&System::OnUpdate, this, ts...));
    }

    template<typename... Cs>
    void System<Cs...>::Update()
    {
        UpdateBuilder<1>(std::placeholders::_1);
    }
}
