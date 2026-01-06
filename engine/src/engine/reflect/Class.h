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
            bool serialized;
        };

        struct Function
        {
            const char* name = nullptr;
            Type* return_type = nullptr;
            std::vector<Type*> argument_types = {};
            std::any func_ptr = nullptr;
        };

        std::vector<Member> members;
        std::vector<Function> functions = {};

        Class();

        Type* GetMemberType(const std::string& fieldName);
        const Member* GetMember(const std::string& fieldName);
        const char* GetMemberName(int i);

        asset::binary::StructLayout GetStructLayout(const void* obj) const override;
        bool IsClass() const override { return true; }

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;

    };
}
