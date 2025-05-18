#pragma once

#include "Type.h"
#include "TypeResolver.h"
#include "Class.h"

namespace se::reflect
{
    struct Type;
    struct Class;

/// Class ///
#define DECLARE_SPARK_POD_CLASS(Type) \
public:               \
static size_t s_StaticId;                         \
static constexpr bool s_IsPOD = true;\
static ::se::reflect::Class* GetReflection(); \
static void InitMembers();

#define DECLARE_SPARK_CLASS(type, ...) \
public:               \
static size_t s_StaticId;                          \
static constexpr bool s_IsPOD = false;\
virtual reflect::Type* GetReflectType() const override\
{\
return reflect::TypeResolver<type>::get();\
}\
virtual void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) override\
{\
reflect::TypeResolver<type>::get()->Serialize(obj, parentObj, fieldName);\
}\
virtual void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) override\
{\
reflect::TypeResolver<type>::get()->Deserialize(obj, parentObj, fieldName);\
}\
virtual asset::binary::StructLayout GetStructLayout(const void*) const override\
{\
return reflect::TypeResolver<type>::get()->GetStructLayout(nullptr);\
}\
virtual std::string GetTypeName() const override\
{\
return reflect::TypeResolver<type>::get()->GetTypeName(nullptr);\
}\
static se::reflect::Class* GetReflection(); \
static void InitMembers();

#define DECLARE_SPARK_COMPONENT(Type) \
DECLARE_SPARK_CLASS(Type)     \
static se::ecs::Id s_ComponentId;\
static se::ecs::Id GetComponentId() { SPARK_ASSERT(s_ComponentId != static_cast<uint64_t>(0), "GetComponentId called before RegisterComponent"); return s_ComponentId; } \
static constexpr bool IsSingletonComponent() { return false; }

#define DECLARE_SPARK_WIDGET_COMPONENT(Type) \
DECLARE_SPARK_CLASS(Type)     \
static se::ecs::Id s_ComponentId;\
static se::ecs::Id GetComponentId() { SPARK_ASSERT(s_ComponentId != static_cast<uint64_t>(0), "GetComponentId called before RegisterComponent"); return s_ComponentId; } \
static constexpr bool IsSingletonComponent() { return false; }

#define DECLARE_SPARK_SINGLETON_COMPONENT(Type) \
DECLARE_SPARK_CLASS(Type)     \
static se::ecs::Id s_ComponentId;\
static se::ecs::Id GetComponentId() { return s_ComponentId; } \
static constexpr bool IsSingletonComponent() { return true; }

#define DECLARE_SPARK_SYSTEM(Type) \
DECLARE_SPARK_CLASS(Type)

#define DECLARE_SPARK_CLASS_TEMPLATED(type, templateTypes) \
public:               \
static size_t s_StaticId;                         \
static constexpr bool s_IsPOD = false;\
virtual reflect::Type* GetReflectType() const override\
{\
return reflect::TypeResolver<type>::get();\
}\
virtual void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) override\
{\
reflect::TypeResolver<type>::get()->Serialize(obj, parentObj, fieldName);\
}\
virtual void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) override\
{\
reflect::TypeResolver<type>::get()->Deserialize(obj, parentObj, fieldName);\
}\
virtual asset::binary::StructLayout GetStructLayout(const void*) const override\
{\
return reflect::TypeResolver<type>::get()->GetStructLayout(nullptr);\
}\
virtual std::string GetTypeName() const override\
{\
return reflect::TypeResolver<type>::get()->GetTypeName(nullptr);\
}\
static se::reflect::Class* GetReflection(); \
static void InitMembers();

#define SPARK_MEMBER(...)

template <typename T>
std::enable_if_t<std::is_default_constructible_v<T>, void> CreateDefaultConstructorMethods(se::reflect::Class* typeDesc)
{
    typeDesc->heap_constructor = [typeDesc]{ return typeDesc->has_default_constructor ? new T() : nullptr; };        \
    typeDesc->inplace_constructor = [typeDesc](void* mem){ return typeDesc->has_default_constructor ? new(mem) T() : nullptr; }; \
}

template <typename T>
std::enable_if_t<!std::is_default_constructible_v<T>, void> CreateDefaultConstructorMethods(se::reflect::Class*)
{
    // ...
}

#define DEFINE_SPARK_TYPE(Type) \
static_assert(std::is_convertible<Type*, se::reflect::ObjectBase*>::value, "Reflectable types must inherit from reflect::ObjectBase");\
size_t Type::s_StaticId = typeid(Type).hash_code();

#define DEFINE_SPARK_TYPE_TEMPLATED(Type, templateTypes, templateParameters)\
template <templateParameters>\
size_t Type<templateTypes>::s_StaticId = typeid(Type<templateTypes>).hash_code();

#define TEMPLATE_TYPES(...) __VA_ARGS__
#define TEMPLATE_PARAMETERS(...) __VA_ARGS__
#define TO_STRING(val) #val

#define DEFINE_SPARK_CLASS_TEMPLATED_BEGIN(type, templateTypes, templateParameters) \
DEFINE_SPARK_TYPE_TEMPLATED(type, templateTypes, templateParameters)                                   \
    template <templateParameters>\
    se::reflect::Class* type<templateTypes>::GetReflection() \
    {\
        static se::reflect::TemplatedClass<templateTypes>* s_Reflection = nullptr;\
        if (!s_Reflection)\
        {\
            s_Reflection = new se::reflect::TemplatedClass<templateTypes>();\
            static_assert(std::is_convertible<type<templateTypes>*, se::reflect::ObjectBase*>::value, "Reflectable types must inherit from reflect::ObjectBase");\
            s_Reflection->name = #type;\
            se::reflect::TypeLookup::GetTypeMap()[s_Reflection->GetTypeName(nullptr)] = s_Reflection;\
            s_Reflection->size = sizeof(type<templateTypes>);                           \
            s_Reflection->heap_constructor = []{ return new type<templateTypes>(); };        \
            s_Reflection->inplace_constructor = [](void* mem){ return new(mem) type<templateTypes>(); }; \
            s_Reflection->heap_copy_constructor = [](void* other){ return new type<templateTypes>(*reinterpret_cast<type<templateTypes>*>(other)); }; \
            s_Reflection->inplace_copy_constructor = [](void* mem, void* other){ return new(mem) type<templateTypes>(*reinterpret_cast<type<templateTypes>*>(other)); }; \
            s_Reflection->destructor = [](void* data){ reinterpret_cast<type<templateTypes>*>(data)->~type(); };        \
            InitMembers();\
        }\
        return s_Reflection;\
    }\
    template <templateParameters>\
    void type<templateTypes>::InitMembers() \
    {\
        GetReflection()->members = {

#define DEFINE_SERIALIZED_MEMBER_TEMPLATED(type, name, templateTypes) \
{#name, se::reflect::TypeResolver<decltype(name)>::get(), [](const void* obj){ return (void*)&((type<templateTypes>*)obj)->name; }, true},

#define DEFINE_MEMBER_TEMPLATED(type, name, templateTypes) \
{#name, se::reflect::TypeResolver<decltype(name)>::get(), [](const void* obj){ return (void*)&((type<templateTypes>*)obj)->name; }, false},

#define DEFINE_SPARK_CLASS_TEMPLATED_END(type) \
};\
}\

#define INIT_TEMPLATED_CLASS_REFLECTION_ONE_ARG(type, templateArg) \
namespace\
{ static auto SPARK_CAT(type, SPARK_CAT(templateArg, Reflection)) = type<templateArg>::GetReflection(); }

/// Enum ///
#define DECLARE_SPARK_ENUM_BEGIN(_enum, type) \
struct _enum : reflect::ObjectBase\
{                                             \
static size_t s_StaticId;                      \
static constexpr bool s_IsPOD = true;\
enum Type : type;\
static std::string ToString(_enum::Type val); \
static _enum::Type FromString(const std::string& str); \
static size_t ValuesCount();\
static se::reflect::Enum* GetReflection(); \
enum Type : type\
{\

#define DECLARE_SPARK_ENUM_END()\
};\
};
}