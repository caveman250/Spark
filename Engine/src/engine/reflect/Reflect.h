#pragma once

#include "spark.h"
#include "Type.h"
#include "TypeResolver.h"
#include "Class.h"
#include "TemplatedClass.h"
#include "Enum.h"
#include "Containers.h"

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
static se::reflect::Class Reflection; \
static void initReflection(se::reflect::Class*);

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
std::string GetTypeName() const\
{\
return reflect::TypeResolver<type>::get()->GetTypeName(nullptr);\
}\
static se::reflect::TemplatedClass<templateTypes> Reflection; \
static void initReflection(se::reflect::TemplatedClass<templateTypes>*);

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
    std::string GetTypeName() const\
    {\
        return reflect::TypeResolver<type>::get()->GetTypeName(nullptr);\
    }\
static se::reflect::Class Reflection; \
static void initReflection(se::reflect::Class*);

#define DEFINE_ABSTRACT_SPARK_CLASS_BEGIN(type) \
DEFINE_SPARK_TYPE(type)                                   \
se::reflect::Class type::Reflection{type::initReflection}; \
void type::initReflection(se::reflect::Class* typeDesc) { \
using VarType = type; \
se::reflect::TypeLookup::GetTypeMap()[#type] = typeDesc;\
typeDesc->name = #type; \
typeDesc->size = sizeof(VarType);                           \
typeDesc->heap_constructor = nullptr;        \
typeDesc->inplace_constructor = nullptr; \
typeDesc->heap_copy_constructor = nullptr; \
typeDesc->inplace_copy_constructor = nullptr; \
typeDesc->destructor = nullptr;      \
typeDesc->members = {

#define DEFINE_SPARK_CLASS_BEGIN(type) \
DEFINE_SPARK_TYPE(type)                                   \
se::reflect::Class type::Reflection{type::initReflection}; \
void type::initReflection(se::reflect::Class* typeDesc) { \
using VarType = type; \
se::reflect::TypeLookup::GetTypeMap()[#type] = typeDesc;\
typeDesc->name = #type; \
typeDesc->size = sizeof(VarType);                           \
typeDesc->heap_constructor = []{ return new VarType(); };        \
typeDesc->inplace_constructor = [](void* mem){ return new(mem) VarType(); }; \
typeDesc->heap_copy_constructor = [](void* other){ return new VarType(*reinterpret_cast<VarType*>(other)); }; \
typeDesc->inplace_copy_constructor = [](void* mem, void* other){ return new(mem) VarType(*reinterpret_cast<VarType*>(other)); }; \
typeDesc->destructor = [](void* data){ reinterpret_cast<VarType*>(data)->~VarType(); };        \
typeDesc->members = {

#define DEFINE_SPARK_POD_CLASS_BEGIN(type) \
DEFINE_SPARK_POD_TYPE(type)                                   \
se::reflect::Class type::Reflection{type::initReflection}; \
void type::initReflection(se::reflect::Class* typeDesc) { \
using VarType = type; \
se::reflect::TypeLookup::GetTypeMap()[#type] = typeDesc;\
typeDesc->name = #type; \
typeDesc->size = sizeof(VarType);                           \
typeDesc->heap_constructor = []{ return new VarType(); };        \
typeDesc->inplace_constructor = [](void* mem){ return new(mem) VarType(); }; \
typeDesc->heap_copy_constructor = [](void* other){ return new VarType(*reinterpret_cast<VarType*>(other)); }; \
typeDesc->inplace_copy_constructor = [](void* mem, void* other){ return new(mem) VarType(*reinterpret_cast<VarType*>(other)); }; \
typeDesc->destructor = [](void* data){ reinterpret_cast<VarType*>(data)->~VarType(); };        \
typeDesc->members = {

#define TEMPLATE_TYPES(...) __VA_ARGS__
#define TEMPLATE_PARAMETERS(...) __VA_ARGS__
#define TO_STRING(val) #val

#define DEFINE_SPARK_CLASS_TEMPLATED_BEGIN(type, templateTypes, templateParameters) \
DEFINE_SPARK_TYPE_TEMPLATED(type, templateTypes, templateParameters)                                   \
template<templateParameters>\
se::reflect::TemplatedClass<templateTypes> type<templateTypes>::Reflection{type::initReflection}; \
template<templateParameters>\
void type<templateTypes>::initReflection(se::reflect::TemplatedClass<templateTypes>* typeDesc) { \
static_assert(std::is_convertible<type<templateTypes>*, se::reflect::ObjectBase*>::value, "Reflectable types must inherit from reflect::ObjectBase");\
typeDesc->name = #type;\
se::reflect::TypeLookup::GetTypeMap()[typeDesc->GetTypeName(nullptr)] = typeDesc;\
typeDesc->size = sizeof(type<templateTypes>);                           \
typeDesc->heap_constructor = []{ return new type<templateTypes>(); };        \
typeDesc->inplace_constructor = [](void* mem){ return new(mem) type<templateTypes>(); }; \
typeDesc->heap_copy_constructor = [](void* other){ return new type<templateTypes>(*reinterpret_cast<type<templateTypes>*>(other)); }; \
typeDesc->inplace_copy_constructor = [](void* mem, void* other){ return new(mem) type<templateTypes>(*reinterpret_cast<type<templateTypes>*>(other)); }; \
typeDesc->destructor = [](void* data){ reinterpret_cast<type<templateTypes>*>(data)->~type(); };        \
typeDesc->members = {

#define DEFINE_MEMBER(name) \
{#name, se::reflect::TypeResolver<decltype(VarType::name)>::get(), [](const void* obj){ return (void*)&((VarType*)obj)->name; }},

#define DEFINE_MEMBER_TEMPLATED(type, name, templateTypes) \
{#name, se::reflect::TypeResolver<decltype(name)>::get(), [](const void* obj){ return (void*)&((type<templateTypes>*)obj)->name; }},

#define DEFINE_SPARK_CLASS_END() \
}; };                    \

    /// Component ///
    // atm just enables registration in ECS. may be expanded later
#define DECLARE_SPARK_COMPONENT(Type) \
DECLARE_SPARK_CLASS(Type)     \
static size_t GetComponentId() { return s_StaticId; } \
static constexpr bool IsSingletonComponent() { return false; }

#define DEFINE_SPARK_COMPONENT_BEGIN(type) \
DEFINE_SPARK_CLASS_BEGIN(type)         \

#define DEFINE_SPARK_COMPONENT_END() \
DEFINE_SPARK_CLASS_END()

#define DECLARE_SPARK_SINGLETON_COMPONENT(Type) \
DECLARE_SPARK_CLASS(Type)     \
static size_t GetComponentId() { return s_StaticId; } \
static constexpr bool IsSingletonComponent() { return true; }

#define DEFINE_SPARK_SINGLETON_COMPONENT_BEGIN(type) \
DEFINE_SPARK_CLASS_BEGIN(type)         \

#define DEFINE_SPARK_SINGLETON_COMPONENT_END() \
DEFINE_SPARK_CLASS_END()

    /// System ///
#define DECLARE_SPARK_SYSTEM(Type) \
DECLARE_SPARK_CLASS(Type) \
static size_t GetSystemId() { return s_StaticId; }

#define DEFINE_SPARK_SYSTEM(type) \
DEFINE_SPARK_CLASS_BEGIN(type) \
DEFINE_SPARK_CLASS_END()

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
static se::reflect::Enum Reflection; \
static void initReflection(se::reflect::Enum*);\
enum Type : type\
{\

#define DECLARE_SPARK_ENUM_END()\
};\
};

#define DEFINE_SPARK_ENUM_BEGIN(type) \
DEFINE_SPARK_TYPE(type)                                      \
se::reflect::Enum type::Reflection{type::initReflection}; \
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
void type::initReflection(se::reflect::Enum* typeDesc) { \
se::reflect::TypeLookup::GetTypeMap()[#type] = typeDesc;\
using VarType = type; \
typeDesc->name = #type; \
typeDesc->size = sizeof(VarType); \
typeDesc->values = {

#define DEFINE_ENUM_VALUE(_enum, name) \
{#name, _enum::name},

#define DEFINE_SPARK_ENUM_END() \
       }; };
};