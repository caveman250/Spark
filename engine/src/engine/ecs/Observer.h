#pragma once
#include "ecs_fwd.h"

namespace se::ecs
{
    class BaseObserver
    {
    public:
        BaseObserver() = default;
        virtual ~BaseObserver()  = default;
    private:
        virtual void Added(const Id& entity, void* component) = 0;
        virtual void Removed(const Id& entity, void* component) = 0;

        friend class World;
    };

    template <typename T>
    class Observer : public BaseObserver
    {
    public:
        Observer() = default;
        ~Observer() override = default;
    protected:
        virtual void OnAdded(const Id& entity, T* component) = 0;
        virtual void OnRemoved(const Id& entity, T* component) = 0;

    private:
        void Added(const Id& entity, void *component) override { OnAdded(entity, static_cast<T*>(component)); }
        void Removed(const Id& entity, void *component) override { OnRemoved(entity, static_cast<T*>(component)); }
    };
}
