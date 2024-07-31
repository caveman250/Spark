#pragma once
#include "Type.h"

namespace se::reflect
{
    struct Class : Type
    {
        struct Member
        {
            const char* name;
            Type* type;
            std::function<void*(const void*)> get;
        };

        std::vector<Member> members;

        Class();
        Class(void (*init)(Class*));

        Type* GetMemberType(const std::string& fieldName);
        const char* GetMemberName(int i);

        asset::binary::StructLayout GetStructLayout(const void* obj) const override;

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;

    };
}
