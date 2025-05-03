#pragma once

#include "spark.h"
#include "engine/asset/binary/binary.h"

namespace se::asset::binary
{
    class Object;
}

namespace se::reflect
{
    struct Type
    {
        std::string name;
        size_t size;
        bool has_default_constructor;
        std::function<void*()> heap_constructor;
        std::function<void*(void*)> inplace_constructor;
        std::function<void*(void*)> heap_copy_constructor;
        std::function<void*(void*, void*)> inplace_copy_constructor;
        std::function<void(void*)> destructor;

        Type(const std::string& name, size_t size, asset::binary::Type binaryType) : name(name), size(size), binaryType(binaryType) {}
        virtual ~Type() {}
        virtual std::string GetTypeName(const void*) const { return name; }
        virtual size_t GetTypeSize() const { return size; }
        virtual asset::binary::Type GetBinaryType() const { return binaryType; }
        virtual bool IsPolymorphic() const { return false; }
        virtual bool IsContainer() const { return false; }
        virtual void* Instantiate(const std::string&, void*) const { return nullptr; }
        virtual bool RequiresExplicitInstantiationWithinClass() const { return false; }

        virtual void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const = 0;
        virtual void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const = 0;
        virtual asset::binary::StructLayout GetStructLayout(const void*) const = 0;

    private:
        asset::binary::Type binaryType;
    };
}
