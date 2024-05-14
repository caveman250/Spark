#pragma once

#include "spark.h"
#include "Type.h"
#include "TypeResolver.h"
#include "Class.h"
#include "Enum.h"

namespace se::reflect
{
    /// Class ///
#define DECLARE_SPARK_CLASS() \
    static se::reflect::Class Reflection; \
    static void initReflection(se::reflect::Class*);

#define DEFINE_SPARK_CLASS_BEGIN(type) \
    se::reflect::Class type::Reflection{type::initReflection}; \
    void type::initReflection(se::reflect::Class* typeDesc) { \
        using T = type; \
        typeDesc->name = #type; \
        typeDesc->size = sizeof(T); \
        typeDesc->members = {

#define DEFINE_MEMBER(name) \
            {#name, offsetof(T, name), se::reflect::TypeResolver<decltype(T::name)>::get()},

#define DEFINE_SPARK_CLASS_END() \
        }; }; \

        /// Enum ///
#define DECLARE_SPARK_ENUM_BEGIN(_enum, type) \
struct _enum\
{\
enum Type : type;\
static std::string ToString(_enum::Type val); \
static _enum::Type FromString(const std::string& str); \
static se::reflect::Enum Reflection; \
static void initReflection(se::reflect::Enum*);\
enum Type : type\
{\

#define DECLARE_SPARK_ENUM_END()\
};\
};

#define DEFINE_SPARK_ENUM_BEGIN(type) \
se::reflect::Enum type::Reflection{type::initReflection}; \
std::string type::ToString(type::Type val) {\
se::reflect::Enum* enumReflection = se::reflect::EnumResolver<type>::get();\
return enumReflection->ToString(val);\
}\
type::Type type::FromString(const std::string& str) {\
se::reflect::Enum* enumReflection = se::reflect::EnumResolver<type>::get();\
return static_cast<type::Type>(enumReflection->FromString(str));\
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