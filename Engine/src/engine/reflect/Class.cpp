#include "Class.h"

#include "engine/asset/binary/Object.h"
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

    asset::binary::StructLayout Class::GetStructLayout() const
    {
        asset::binary::StructLayout structLayout;
        for (const Member& member : members)
        {
            structLayout.push_back( { asset::binary::CreateFixedString32(member.name), member.type->binaryType });
        }
        return structLayout;
    }

    void Class::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        std::optional<asset::binary::Object> binaryObj = std::nullopt;

        if (!fieldName.empty()) // parent is an object
        {
            asset::binary::StructLayout structLayout = GetStructLayout();
            for (const Member& member : members)
            {
                structLayout.push_back( { asset::binary::CreateFixedString32(member.name), member.type->binaryType });
            }
            auto db = parentObj.GetDatabase();
            auto structIndex = db->GetOrCreateStruct(structLayout);
            binaryObj = db->CreateObject(structIndex);
        }
        else // parent is an array
        {
            binaryObj = parentObj;
        }

        for (size_t i = 0; i < members.size(); ++i)
        {
            const Member& member = members[i];
            member.type->Serialize(static_cast<const char*>(obj) + member.offset, binaryObj.value(), member.name);
        }

        if (!fieldName.empty())
        {
            parentObj.Set(fieldName, binaryObj.value());
        }
    }

    void Class::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        // obj here is already allocated
        asset::binary::Object thisObj = fieldName.empty() ? parentObj : parentObj.Get<asset::binary::Object>(fieldName);
        for (size_t i = 0; i < members.size(); ++i)
        {
            members[i].type->Deserialize((char*)obj + members[i].offset, thisObj, members[i].name);
        }
    }
}
