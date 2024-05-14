#include "Class.h"

#include "engine/asset/binary/Type.h"

namespace se::reflect
{
    Class::Class(void(* init)(Class*)): Type{nullptr, 0, asset::binary::Type::Object }
    {
        init(this);
    }

    Class::Class(const char*, size_t, const std::initializer_list<Member>& init): Type{ nullptr, 0, asset::binary::Type::Object }
        , members{ init }
    {
    }

    Type* Class::GetMemberType(const std::string& fieldName)
    {
        auto it = std::ranges::find_if(members, [fieldName](const Member& member){ return member.name == fieldName; });
        if (it != members.end())
        {
            return (*it).type;
        }

        return nullptr;
    }

    const char* Class::GetMemberName(int i)
    {
        return members[i].name;
    }
}
