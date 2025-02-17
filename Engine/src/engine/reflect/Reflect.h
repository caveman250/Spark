#pragma once

#include "spark.h"
#include "Type.h"
#include "TypeResolver.h"
#include "Class.h"
#include "TemplatedClass.h"
#include "Enum.h"
#include "Containers.h"
#include "engine/bits/PackUtil.h"

namespace se::reflect
{
    struct TypeLookup
    {
        static Type* GetType(const std::string& type) { SPARK_ASSERT(GetTypeMap().contains(type)); return GetTypeMap().at(type); }
        // Warning: Accessed during static initialization.
        static std::unordered_map<std::string, Type*>& GetTypeMap()
        {
            static std::unordered_map<std::string, Type*> typeMap;
            return typeMap;
        }
    };

#define DECLARE_SPARK_TYPE(Type) \
static size_t s_StaticId;

#define DEFINE_SPARK_POD_TYPE(Type) \
size_t Type::s_StaticId = typeid(Type).hash_code();

#define DEFINE_SPARK_TYPE(Type) \
static_assert(std::is_convertible<Type*, se::reflect::ObjectBase*>::value, "Reflectable types must inherit from reflect::ObjectBase");\
size_t Type::s_StaticId = typeid(Type).hash_code();

#define DEFINE_SPARK_TYPE_TEMPLATED(Type, templateTypes, templateParameters)\
template <templateParameters>\
size_t Type<templateTypes>::s_StaticId = typeid(Type<templateTypes>).hash_code();

    /// Class ///

#define DECLARE_SPARK_POD_CLASS(Type) \
public:               \
DECLARE_SPARK_TYPE(Type)                          \
static constexpr bool s_IsPOD = true;\
static se::reflect::Class* GetReflection(); \

#define DECLARE_SPARK_CLASS_TEMPLATED(type, templateTypes) \
public:               \
DECLARE_SPARK_TYPE(type)                          \
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

#define DECLARE_SPARK_CLASS(type) \
public:               \
DECLARE_SPARK_TYPE(type)                          \
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

#define DEFINE_ABSTRACT_SPARK_CLASS_BEGIN(type) \
DEFINE_SPARK_TYPE(type)                                   \
se::reflect::Class* type::GetReflection() \
{\
    static se::reflect::Class* s_Reflection = nullptr;\
    if (!s_Reflection)\
    {\
        s_Reflection = new se::reflect::Class();\
        using VarType = type; \
        se::reflect::TypeLookup::GetTypeMap()[#type] = s_Reflection;\
        s_Reflection->name = #type; \
        s_Reflection->size = sizeof(VarType);                           \
        s_Reflection->heap_constructor = nullptr;        \
        s_Reflection->inplace_constructor = nullptr; \
        s_Reflection->heap_copy_constructor = nullptr; \
        s_Reflection->inplace_copy_constructor = nullptr; \
        s_Reflection->destructor = nullptr;      \
        s_Reflection->members = {

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

#define DEFINE_SPARK_CLASS_BEGIN(type) \
DEFINE_SPARK_TYPE(type)                                   \
se::reflect::Class* type::GetReflection() \
{\
static se::reflect::Class* s_Reflection = nullptr;\
if (!s_Reflection)\
{\
s_Reflection = new se::reflect::Class();\
using VarType = type; \
se::reflect::TypeLookup::GetTypeMap()[#type] = s_Reflection;\
s_Reflection->name = #type; \
s_Reflection->size = sizeof(VarType);                           \
s_Reflection->has_default_constructor = std::is_default_constructible<type>::value; \
CreateDefaultConstructorMethods<type>(s_Reflection);\
s_Reflection->heap_copy_constructor = [](void* other){ return new VarType(*reinterpret_cast<VarType*>(other)); }; \
s_Reflection->inplace_copy_constructor = [](void* mem, void* other){ return new(mem) VarType(*reinterpret_cast<VarType*>(other)); }; \
s_Reflection->destructor = [](void* data){ reinterpret_cast<VarType*>(data)->~VarType(); };        \
s_Reflection->members = {

#define DEFINE_SPARK_POD_CLASS_BEGIN(type) \
DEFINE_SPARK_POD_TYPE(type)                                   \
se::reflect::Class* type::GetReflection() \
{\
static se::reflect::Class* s_Reflection = nullptr;\
if (!s_Reflection)\
{\
s_Reflection = new se::reflect::Class();\
using VarType = type; \
se::reflect::TypeLookup::GetTypeMap()[#type] = s_Reflection;\
s_Reflection->name = #type; \
s_Reflection->size = sizeof(VarType);                           \
s_Reflection->heap_constructor = []{ return new VarType(); };        \
s_Reflection->inplace_constructor = [](void* mem){ return new(mem) VarType(); }; \
s_Reflection->heap_copy_constructor = [](void* other){ return new VarType(*reinterpret_cast<VarType*>(other)); }; \
s_Reflection->inplace_copy_constructor = [](void* mem, void* other){ return new(mem) VarType(*reinterpret_cast<VarType*>(other)); }; \
s_Reflection->destructor = [](void* data){ reinterpret_cast<VarType*>(data)->~VarType(); };        \
s_Reflection->members = {

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
s_Reflection->members = {

#define DEFINE_SERIALIZED_MEMBER(name) \
{#name, se::reflect::TypeResolver<decltype(VarType::name)>::get(), [](const void* obj){ return (void*)&((VarType*)obj)->name; }, true},

#define DEFINE_SERIALIZED_MEMBER_TEMPLATED(type, name, templateTypes) \
{#name, se::reflect::TypeResolver<decltype(name)>::get(), [](const void* obj){ return (void*)&((type<templateTypes>*)obj)->name; }, true},

#define DEFINE_MEMBER(name) \
{#name, se::reflect::TypeResolver<decltype(VarType::name)>::get(), [](const void* obj){ return (void*)&((VarType*)obj)->name; }, false},

#define DEFINE_MEMBER_TEMPLATED(type, name, templateTypes) \
{#name, se::reflect::TypeResolver<decltype(name)>::get(), [](const void* obj){ return (void*)&((type<templateTypes>*)obj)->name; }, false},


#define DEFINE_SPARK_CLASS_END(type) \
};\
}\
return s_Reflection;\
}\
namespace\
{ static auto SPARK_CAT(type, Reflection) = type::GetReflection(); } // ensure reflection has been initialised.

#define DEFINE_SPARK_CLASS_TEMPLATED_END(type) \
};\
}\
return s_Reflection;\
}\

#define INIT_TEMPLATED_CLASS_REFLECTION_ONE_ARG(type, templateArg) \
namespace\
{ static auto SPARK_CAT(type, SPARK_CAT(templateArg, Reflection)) = type<templateArg>::GetReflection(); }

    /// Component ///
#define DECLARE_SPARK_COMPONENT(Type) \
DECLARE_SPARK_CLASS(Type)     \
static se::ecs::Id s_ComponentId;\
static se::ecs::Id GetComponentId() { SPARK_ASSERT(s_ComponentId != static_cast<uint64_t>(0), "GetComponentId called before RegisterComponent"); return s_ComponentId; } \
static constexpr bool IsSingletonComponent() { return false; }

#define DEFINE_SPARK_COMPONENT_BEGIN(type) \
se::ecs::Id type::s_ComponentId = {};\
DEFINE_SPARK_CLASS_BEGIN(type)         \


#define DEFINE_SPARK_COMPONENT_END(type) \
DEFINE_SPARK_CLASS_END(type)

#define DECLARE_SPARK_SINGLETON_COMPONENT(Type) \
DECLARE_SPARK_CLASS(Type)     \
static se::ecs::Id s_ComponentId;\
static se::ecs::Id GetComponentId() { return s_ComponentId; } \
static constexpr bool IsSingletonComponent() { return true; }

#define DEFINE_SPARK_SINGLETON_COMPONENT_BEGIN(type) \
se::ecs::Id type::s_ComponentId = {};\
DEFINE_SPARK_CLASS_BEGIN(type)         \

#define DEFINE_SPARK_SINGLETON_COMPONENT_END(type) \
DEFINE_SPARK_CLASS_END(type)

    /// System ///
#define DECLARE_SPARK_SYSTEM(Type) \
DECLARE_SPARK_CLASS(Type) \

#define DEFINE_SPARK_SYSTEM(type) \
DEFINE_SPARK_CLASS_BEGIN(type) \
DEFINE_SPARK_CLASS_END(type)

    /// Enum ///
#define DECLARE_SPARK_ENUM_BEGIN(_enum, type) \
struct _enum : reflect::ObjectBase\
{                                             \
DECLARE_SPARK_TYPE(_enum)                       \
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

#define DEFINE_SPARK_ENUM_BEGIN(type) \
DEFINE_SPARK_TYPE(type)                                      \
std::string type::ToString(type::Type val) {\
se::reflect::Enum* enumReflection = se::reflect::EnumResolver<type>::get();\
return enumReflection->ToString(val);\
}\
type::Type type::FromString(const std::string& str) {\
se::reflect::Enum* enumReflection = se::reflect::EnumResolver<type>::get();\
return static_cast<type::Type>(enumReflection->FromString(str));\
}\
    namespace se::reflect {\
    template <>\
    Type* getPrimitiveDescriptor<type::Type>()\
    {\
        return TypeResolver<type>::get();\
    }\
}\
size_t type::ValuesCount() {\
se::reflect::Enum* enumReflection = se::reflect::EnumResolver<type>::get();\
return enumReflection->values.size();\
}\
se::reflect::Enum* type::GetReflection() { \
static se::reflect::Enum* s_Instance = nullptr;\
if (!s_Instance)\
{\
s_Instance = new se::reflect::Enum();\
se::reflect::TypeLookup::GetTypeMap()[#type] = s_Instance;\
using VarType = type; \
s_Instance->name = #type; \
s_Instance->size = sizeof(VarType); \
s_Instance->values = {

#define DEFINE_ENUM_VALUE(_enum, name) \
{#name, _enum::name},

#define DEFINE_SPARK_ENUM_END() \
       };\
    }\
return s_Instance;\
}
};