#include "Reflect.h"
#include "engine/asset/binary/Type.h"

namespace se::reflect
{
    struct Type_StdVector : Type
    {
        Type* itemType;

        size_t (*getSize)(const void*);
        const void* (*getItem)(const void*, size_t);

        template <typename T>
        Type_StdVector(T*)
            : Type{ "std::vector<>", sizeof(std::vector<T>), asset::binary::Type::Array },
              itemType{ TypeResolver<T>::get() }
        {
            getSize = [](const void* vecPtr)
            {
                const auto& vec = *static_cast<const std::vector<T>*>(vecPtr);
                return vec.size();
            };

            getItem = [](const void* vecPtr, size_t index)
            {
                const auto& vec = *static_cast<const std::vector<T>*>(vecPtr);
                return &vec[index];
            };
        }

        virtual std::string GetTypeName() const override
        {
            return std::string("std::vector<") + itemType->GetTypeName() + ">";
        }
    };

    template <typename T>
    class TypeResolver<std::vector<T> >
    {
    public:
        static Type* get()
        {
            static Type_StdVector typeDesc{(T*) nullptr};
            return &typeDesc;
        }
    };
}
