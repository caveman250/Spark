#pragma once

namespace se::ecs
{
    typedef uint64_t Id;

    class BaseObserver
    {
    public:
        BaseObserver() = default;
        virtual ~BaseObserver()  = default;
    private:
        virtual void Added(Id entity, void* component) = 0;
        virtual void Removed(Id entity, void* component) = 0;

        friend class World;
    };

    template <typename T>
    class Observer : public BaseObserver
    {
    public:
        Observer() = default;
        ~Observer() override = default;
    protected:
        virtual void OnAdded(Id entity, T* component) = 0;
        virtual void OnRemoved(Id entity, T* component) = 0;

    private:
        void Added(Id entity, void *component) override { OnAdded(entity, static_cast<T*>(component)); }
        void Removed(Id entity, void *component) override { OnRemoved(entity, static_cast<T*>(component)); }
    };
}
