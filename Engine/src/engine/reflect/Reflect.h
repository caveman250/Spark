#pragma once

#include "spark.h"
#include "Type.h"
#include "TypeResolver.h"
#include "Class.h"
#include "Enum.h"
#include "Containers.h"

namespace se::reflect
{
#define DECLARE_SPARK_TYPE(Type) \
static size_t s_StaticId;

#define DEFINE_SPARK_TYPE(Type) \
size_t Type::s_StaticId = typeid(Type).hash_code();

    /// Class ///
#define DECLARE_SPARK_CLASS(Type) \
public:               \
DECLARE_SPARK_TYPE(Type)                          \
static se::reflect::Class Reflection; \
static void initReflection(se::reflect::Class*);

#define DEFINE_SPARK_CLASS_BEGIN(type) \
DEFINE_SPARK_TYPE(type)                                   \
se::reflect::Class type::Reflection{type::initReflection}; \
void type::initReflection(se::reflect::Class* typeDesc) { \
using T = type; \
typeDesc->name = #type; \
typeDesc->size = sizeof(T);                           \
typeDesc->heap_constructor = []{ return new T(); };        \
typeDesc->inplace_constructor = [](void* mem){ return new(mem) T(); }; \
typeDesc->heap_copy_constructor = [](void* other){ return new T(*reinterpret_cast<T*>(other)); }; \
typeDesc->inplace_copy_constructor = [](void* mem, void* other){ return new(mem) T(*reinterpret_cast<T*>(other)); }; \
typeDesc->destructor = [](void* data){ reinterpret_cast<T*>(data)->~T(); };        \
typeDesc->members = {

#define DEFINE_MEMBER(name) \
{#name, offsetof(T, name), se::reflect::TypeResolver<decltype(T::name)>::get()},

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
struct _enum\
{                                             \
DECLARE_SPARK_TYPE(_enum)                       \
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
using T = type; \
typeDesc->name = #type; \
typeDesc->size = sizeof(T); \
typeDesc->values = {

#define DEFINE_ENUM_VALUE(_enum, name) \
{#name, _enum::name},

#define DEFINE_SPARK_ENUM_END() \
       }; };
};