#include "Object.h"
#include "Database.h"
#include "engine/math/math.h"
#include "engine/memory/BinaryBlob.h"

namespace se::asset::binary
{
    Object::Object(uint32_t offset, Database* database, const Struct& objStruct)
        : m_Offset(offset)
        , m_Struct(objStruct)
        , m_DB(database)
    {

    }

    char* Object::GetData() const
    {
        return m_DB->GetObjectDataAt(m_Offset);
    }

    void CheckType(Type type, [[maybe_unused]] const std::type_index& typeId)
    {
        switch (type)
        {
        case Type::Bool:
            SPARK_ASSERT(typeId == typeid(bool));
            break;
        case Type::Int8:
            SPARK_ASSERT(typeId == typeid(int8_t) || typeId == typeid(char));
            break;
        case Type::Uint8:
            SPARK_ASSERT(typeId == typeid(uint8_t));
            break;
        case Type::Int16:
            SPARK_ASSERT(typeId == typeid(int16_t));
            break;
        case Type::Uint16:
            SPARK_ASSERT(typeId == typeid(uint16_t));
            break;
        case Type::Int32:
            SPARK_ASSERT(typeId == typeid(int32_t));
            break;
        case Type::Uint32:
            SPARK_ASSERT(typeId == typeid(uint32_t));
            break;
        case Type::Int64:
            SPARK_ASSERT(typeId == typeid(int64_t));
            break;
        case Type::Uint64:
            SPARK_ASSERT(typeId == typeid(uint64_t) || typeId == typeid(size_t));
            break;
        case Type::Float:
            SPARK_ASSERT(typeId == typeid(float));
            break;
        case Type::Double:
            SPARK_ASSERT(typeId == typeid(double));
            break;
        case Type::Vec2:
            SPARK_ASSERT(typeId == typeid(math::Vec2));
            break;
        case Type::Vec3:
            SPARK_ASSERT(typeId == typeid(math::Vec3));
            break;
        case Type::Vec4:
            SPARK_ASSERT(typeId == typeid(math::Vec4));
            break;
        case Type::Object:
            SPARK_ASSERT(typeId == typeid(Object));
            break;
        case Type::String:
            SPARK_ASSERT(typeId == typeid(const char*));
            break;
        case Type::Blob:
            SPARK_ASSERT(typeId == typeid(Blob));
            break;
        case Type::Array:
            SPARK_ASSERT(typeId == typeid(binary::Array));
            break;
        case Type::PolymorphicArray:
            SPARK_ASSERT(typeId == typeid(PolymorphicArray));
            break;
        default:
            SPARK_ASSERT(false, "CheckType - Unrecognized type!");
            break;
        }
    }

    std::string Object::GetNativeTypeString(const std::string& field) const
    {
        auto binaryType = m_Struct.GetFieldType(m_Struct.GetFieldIndex(field));
        switch (binaryType)
        {
            case Type::Object:
            {
                auto obj = Get<Object>(field);
                return obj.GetStruct().GetName();
            }
            case Type::Bool:
                return "bool";
            case Type::Int8:
                return "int8_t";
            case Type::Uint8:
                return "uint8_t";
            case Type::Int16:
                return "int16_t";
            case Type::Uint16:
                return "uint16_t";
            case Type::Int32:
                return "int32_t";
            case Type::Uint32:
                return "uint32_t";
            case Type::Int64:
                return "int64_t";
            case Type::Uint64:
                return "uint64_t";
            case Type::Float:
                return "float";
            case Type::Double:
                return "double";
            case Type::Vec2:
                return "Vec2";
            case Type::Vec3:
                return "Vec3";
            case Type::Vec4:
                return "Vec4";
            case Type::String:
                return "string";
            case Type::Blob:
            case Type::Array:
            case Type::PolymorphicArray:
            case Type::Invalid:
            default:
                SPARK_ASSERT("Object::GetNativeTypeString - Invalid Type for deserialisation.");
                return "";
        }
    }

    void Object::Set(uint32_t fieldIndex, const char* data, size_t size)
    {
        std::memcpy(GetData() + m_Struct.GetFieldOffset(fieldIndex), data, size);
    }

    Struct Object::GetStruct(uint32_t structIndex) const
    {
        return Struct(m_DB->GetStructName(structIndex), structIndex, m_DB);
    }

    Object Object::GetObjectAt(uint32_t offset) const
    {
        return m_DB->GetObjectAt(offset);
    }

    Array Object::GetArrayAt(uint32_t offset) const
    {
        return m_DB->GetArrayAt(offset);
    }

    PolymorphicArray Object::GetPolymorphicArrayAt(uint32_t offset) const
    {
        return m_DB->GetPolymorphicArrayAt(offset);
    }

    const char* Object::GetString(const std::string& field) const
    {
#if !SPARK_DIST
        CheckType<const char*>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        uint32_t offset = *reinterpret_cast<uint32_t*>(GetData() + m_Struct.GetFieldOffset(m_Struct.GetFieldIndex(field)));
        return m_DB->GetStringAt(offset);
    }

    void Object::SetString(const std::string& field, const char* val)
    {
#if !SPARK_DIST
        CheckType<const char*>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif

        uint32_t offset = m_DB->CreateString(val);
        Set(m_Struct.GetFieldIndex(field), reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
    }

    uint32_t Object::GetBlobOffset(const Blob& blob) const
    {
        return m_DB->GetBlobOffset(blob);
    }

    Blob Object::GetBlobAt(uint32_t offset) const
    {
        return m_DB->GetBlobAt(offset);
    }

    nlohmann::ordered_json Object::ToJson() const
    {
        nlohmann::ordered_json ret = {};
        nlohmann::ordered_json obj;
        auto objStruct = GetStruct();
        auto fieldCount = objStruct.GetFieldCount();
        for (uint32_t i = 0; i < fieldCount; ++i)
        {
            auto fieldName = objStruct.GetFieldName(i);
            auto fieldType = objStruct.GetFieldType(i);

            switch (fieldType)
            {
            case Type::Bool:
                obj[fieldName] = Get<bool>(fieldName);
                break;
            case Type::Int8:
                obj[fieldName] = Get<int8_t>(fieldName);
                break;
            case Type::Uint8:
                obj[fieldName] = Get<uint8_t>(fieldName);
                break;
            case Type::Int16:
                obj[fieldName] = Get<int16_t>(fieldName);
                break;
            case Type::Uint16:
                obj[fieldName] = Get<uint16_t>(fieldName);
                break;
            case Type::Int32:
                obj[fieldName] = static_cast<int64_t>(Get<int32_t>(fieldName)); // y tho
                break;
            case Type::Uint32:
                obj[fieldName] = Get<uint32_t>(fieldName);
                break;
            case Type::Int64:
                obj[fieldName] = Get<int64_t>(fieldName);
                break;
            case Type::Uint64:
                obj[fieldName] = Get<uint64_t>(fieldName);
                break;
            case Type::Float:
                obj[fieldName] = Get<float>(fieldName);
                break;
            case Type::Double:
                obj[fieldName] = Get<double>(fieldName);
                break;
            case Type::Vec2:
                {
                    nlohmann::ordered_json vecJson = {};
                    math::Vec2 vec = Get<math::Vec2>(fieldName);
                    vecJson["x"] = vec.x;
                    vecJson["y"] = vec.y;
                    obj[fieldName] = vecJson;
                    break;
                }
            case Type::Vec3:
                {
                    nlohmann::ordered_json vecJson = {};
                    math::Vec3 vec = Get<math::Vec3>(fieldName);
                    vecJson["x"] = vec.x;
                    vecJson["y"] = vec.y;
                    vecJson["z"] = vec.z;
                    obj[fieldName] = vecJson;
                    break;
                }
            case Type::Vec4:
                {
                    nlohmann::ordered_json vecJson = {};
                    math::Vec4 vec = Get<math::Vec4>(fieldName);
                    vecJson["x"] = vec.x;
                    vecJson["y"] = vec.y;
                    vecJson["z"] = vec.z;
                    vecJson["w"] = vec.w;
                    obj[fieldName] = vecJson;
                    break;
                }
            case Type::Object:
                {
                    Object binaryObj = Get<Object>(fieldName);
                    obj[fieldName] = binaryObj.ToJson();
                    break;
                }
            case Type::String:
                obj[fieldName] = Get<std::string>(fieldName);
                break;
            case Type::Blob:
                {
                    nlohmann::ordered_json blobJson = {};
                    Blob blob = Get<Blob>(fieldName);
                    for (size_t j = 0; j < blob.GetSize(); ++j)
                    {
                        blobJson.push_back(blob.GetData()[j]);
                    }
                    obj[fieldName] = blobJson;
                    break;
                }
            case Type::Array:
                {
                    nlohmann::ordered_json arrayJson = {};
                    auto array = Get<Array>(fieldName);
                    for (size_t j = 0; j < array.GetCount(); ++j)
                    {
                        nlohmann::ordered_json itemJson = array.Get(j).ToJson();
                        arrayJson.push_back(itemJson);
                    }
                    obj[fieldName] = arrayJson;
                    break;
                }
            case Type::PolymorphicArray:
                {
                    nlohmann::ordered_json arrayJson = {};
                    auto array = Get<PolymorphicArray>(fieldName);
                    for (size_t j = 0; j < array.GetCount(); ++j)
                    {
                        nlohmann::ordered_json itemJson = array.Get(j).ToJson();
                        arrayJson.push_back(itemJson);
                    }
                    obj[fieldName] = arrayJson;
                    break;
                }
            case Type::Invalid:
                SPARK_ASSERT(false);
                break;
            }
        }

        ret[m_Struct.GetName()] = obj;

        return ret;
    }
}
