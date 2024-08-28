#pragma once
#include "SignalActionBuilder.h"
#include "engine/Application.h"

namespace se::ecs
{
    typedef uint64_t Id;
    typedef uint64_t SignalHandle;

    class Signal
    {
    public:
        ~Signal();

        template<typename... Ts>
        SignalHandle Subscribe(Id entityId, std::function<void(Ts*...)>&& func);

        void Unsubscribe(SignalHandle handle);

        void Broadcast() const;

    private:
        void Execute() const;

        SignalHandle m_HandleCounter;
        std::unordered_map<SignalHandle, std::function<void()>> m_RegisteredHandles;

        friend class World;
    };

    template <typename T>
    void CollectComponentId(std::vector<Id>& vec)
    {
        vec.push_back(T::GetComponentId());
    }

    template<typename ... Ts>
    SignalHandle Signal::Subscribe(Id entityId, std::function<void(Ts*...)> &&func)
    {
        SignalHandle id = m_HandleCounter++;

        auto wrapperFunc = [this, entityId, wrappedFunc = std::move(func)]()
        {
            auto* world = Application::Get()->GetWorld();

            std::vector<Id> comps;
            comps.reserve(sizeof...(Ts));
            (CollectComponentId<Ts>(comps), ...);

            for (const auto& comp : comps)
            {
                if (!world->HasComponent(entityId, comp))
                {
                    return;
                }
            }

            SignalActionBuilder<Ts...> builder;
            builder.DoAction(entityId, world, wrappedFunc);
        };

        m_RegisteredHandles.insert(std::make_pair(id, std::move(wrapperFunc)));
        return id;
    }
}
