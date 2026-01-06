#pragma once

#include "Type.h"
#include "Class.h"

#define SPARK_POD_CLASS(...) \
    public:               \
    static size_t s_StaticId;                         \
    static constexpr bool s_IsPOD = true;\
    static ::se::reflect::Class* GetReflection(); \
    static void InitMembers();\

#define SPARK_CLASS(...) \
    public:               \
    static size_t s_StaticId;                          \
    static constexpr bool s_IsPOD = false;\
    virtual reflect::Type* GetReflectType() const override;\
    virtual void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) override;\
    virtual void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) override;\
    virtual asset::binary::StructLayout GetStructLayout(const void*) const override;\
    virtual std::string GetTypeName() const override;\
    virtual void Invoke(const std::string&, const std::vector<std::any>&) override;\
    static se::reflect::Class* GetReflection(); \
    static void InitMembers();

#define SPARK_COMPONENT(...) \
    SPARK_CLASS()     \
    static se::ecs::Id s_ComponentId;\
    static se::ecs::Id GetComponentId() { SPARK_ASSERT(s_ComponentId != static_cast<uint64_t>(0), "GetComponentId called before RegisterComponent"); return s_ComponentId; } \
    static constexpr bool IsSingletonComponent() { return false; }\
    se::ecs::Id GetStaticComponentId() const override { return s_ComponentId; }

#define SPARK_WIDGET_COMPONENT(...) \
    SPARK_CLASS()     \
    static se::ecs::Id s_ComponentId;\
    static se::ecs::Id GetComponentId() { SPARK_ASSERT(s_ComponentId != static_cast<uint64_t>(0), "GetComponentId called before RegisterComponent"); return s_ComponentId; } \
    static constexpr bool IsSingletonComponent() { return false; }\
    se::ecs::Id GetStaticComponentId() const override { return s_ComponentId; }

#define SPARK_SINGLETON_COMPONENT(...) \
    SPARK_CLASS()     \
    static se::ecs::Id s_ComponentId;\
    static se::ecs::Id GetComponentId() { return s_ComponentId; } \
    static constexpr bool IsSingletonComponent() { return true; }\
    se::ecs::Id GetStaticComponentId() const override { return s_ComponentId; }

#define SPARK_SYSTEM(...) \
    SPARK_CLASS()\
    static se::ecs::Id s_SystemId;\
    static se::ecs::Id GetSystemId() { SPARK_ASSERT(s_SystemId != static_cast<uint64_t>(0), "GetSystemId called before RegisterSystem"); return s_SystemId; }

#define SPARK_CLASS_TEMPLATED(...) \
    public:               \
    static size_t s_StaticId;                         \
    static constexpr bool s_IsPOD = false;\
    virtual reflect::Type* GetReflectType() const override;\
    virtual void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) override;\
    virtual void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) override;\
    virtual asset::binary::StructLayout GetStructLayout(const void*) const override;\
    virtual std::string GetTypeName() const override;\
    virtual void Invoke(const std::string&, const std::vector<std::any>&) override; \
    static se::reflect::Class* GetReflection(); \
    static void InitMembers();

    template <typename T>
    concept DefaultConstructible = std::is_default_constructible_v<T>;

    template <DefaultConstructible T>
    void CreateDefaultConstructorMethods(se::reflect::Class* typeDesc)
    {
        typeDesc->heap_constructor = [typeDesc]{ return typeDesc->has_default_constructor ? new T() : nullptr; };        \
        typeDesc->inplace_constructor = [typeDesc](void* mem){ return typeDesc->has_default_constructor ? new(mem) T() : nullptr; }; \
    }

    template <typename T>
    void CreateDefaultConstructorMethods(se::reflect::Class*)
    {
        // ...
    }

// Header tool tags
#define SPARK_MEMBER(...)
#define SPARK_FUNCTION(...)
#define SPARK_INSTANTIATE_TEMPLATE(...)
#define SPARK_ENUM(...)