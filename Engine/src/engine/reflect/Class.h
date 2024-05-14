#pragma once
#include "Type.h"

namespace se::reflect
{
    struct Class : Type
    {
        struct Member
        {
            const char* name;
            size_t offset;
            Type *type;
        };

        std::vector<Member> members;

        Class(void (*init)(Class *));
        Class(const char *, size_t, const std::initializer_list<Member> &init);

        Type* GetMemberType(const std::string &fieldName);
        const char* GetMemberName(int i);
    };
}
