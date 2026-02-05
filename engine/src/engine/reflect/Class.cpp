#include "Class.h"

#include "engine/asset/binary/Object.h"
#include "engine/asset/binary/Type.h"
#include "engine/asset/texture/Mipmap.h"
#include "engine/asset/texture/Texture.h"

namespace se::reflect
{
    Class::Class() : Type{"", 0, asset::binary::Type::Object }
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

    const Class::Member* Class::GetMember(const std::string& fieldName)
    {
        auto it = std::ranges::find_if(members, [fieldName](const Member& member){ return member.name == fieldName; });
        if (it != members.end())
        {
            return &*it;
        }

        return nullptr;
    }

    const char* Class::GetMemberName(int i)
    {
        return members[i].name;
    }

    asset::binary::StructLayout Class::GetStructLayout(const void*) const
    {
        asset::binary::StructLayout structLayout;
        for (const Member& member : members)
        {
            if (member.serialized)
            {
                structLayout.push_back( { asset::binary::CreateFixedString64(member.name), member.type->GetBinaryType() });
            }
        }
        return structLayout;
    }

    void Class::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        std::optional<asset::binary::Object> binaryObj = std::nullopt;

        if (!fieldName.empty()) // parent is an object
        {
            asset::binary::StructLayout structLayout = GetStructLayout(nullptr);
            auto db = parentObj.GetDatabase();
            auto structIndex = db->GetOrCreateStruct(name, structLayout);
            binaryObj = db->CreateObject(structIndex);
        }
        else // parent is this
        {
            binaryObj = parentObj;
        }

        for (size_t i = 0; i < members.size(); ++i)
        {
            const Member& member = members[i];
            if (member.serialized)
            {
                member.type->Serialize(member.get(obj), binaryObj.value(), member.name);
            }
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
            auto& member = members[i];

            if (!thisObj.HasField(member.name))
            {
                continue;
            }

            if (member.serialized)
            {
                if (member.type->RequiresExplicitInstantiation())
                {
                    auto binaryObj = thisObj.Get<asset::binary::Object>(member.name);
                    std::string typeName = binaryObj.GetStruct().GetName();
                    member.type->Instantiate(typeName, member.get(obj));
                }
                members[i].type->Deserialize(members[i].get(obj), thisObj, members[i].name);
            }
        }
    }
}
