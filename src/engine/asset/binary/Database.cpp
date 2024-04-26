#include <engine/io/FileSystem.h>
#include "Database.h"

namespace se::asset::binary
{
    constexpr uint32_t s_StructsHeaderSize = sizeof(uint32_t) * 2; // numStructs (uint32_t), totalStructsSize (uint32_t)]
    constexpr uint32_t s_ObjectsHeaderSize = sizeof(uint32_t); // totalSize (uint32_t)
    constexpr uint32_t s_StringsHeaderSize = sizeof(uint32_t); // totalSize (uint32_t)


    Database::Database(bool readOnly)
            : m_ReadOnly(readOnly)
    {

    }

    Database::~Database()
    {
        if (m_ReadOnly)
        {
            std::free(m_Structs); // all one allocation
        }
        else
        {
            std::free(m_Structs);
            std::free(m_Objects);
        }
    }

    void Database::CreateStructData(const std::vector<std::pair<FixedString32, Type>>& structLayout, void* createAt)
    {
        char* structData = static_cast<char*>(createAt);

        uint32_t structSize = structLayout.size();
        std::memcpy(structData, &structSize, sizeof(uint32_t));

        uint32_t offset = s_StructHeaderSize;
        int index = 0;
        uint32_t rowSize = sizeof(Type) + 32;
        for (const auto& [name, type] : structLayout)
        {
            std::memcpy(structData + offset, &name, 32);
            std::memcpy(structData + offset + 32, &type, sizeof(Type));
            index++;
            offset += rowSize;
        }
    }

    uint32_t Database::CalcStructDefinitionDataSize(const std::vector<std::pair<FixedString32, Type>>& structLayout)
    {
        return s_StructRowSize * structLayout.size() + s_StructHeaderSize;
    }

    uint32_t Database::CreateStruct(const StructLayout& structLayout)
    {
        if (!SPARK_VERIFY(!m_ReadOnly))
        {
            return std::numeric_limits<uint32_t>().max();
        }

        uint32_t reqSize = CalcStructDefinitionDataSize(structLayout);
        if (!m_Structs)
        {
            reqSize += s_StructsHeaderSize;
        }
        uint32_t pos = GrowStructData(reqSize);
        CreateStructData(structLayout, m_Structs + pos);

        return GetNumStructs() - 1;
    }

    uint32_t Database::GetStructsDataSize()
    {
        if (!m_Structs)
        {
            return s_StructsHeaderSize;
        }

        return *(reinterpret_cast<uint32_t*>(m_Structs) + 1);
    }

    uint32_t Database::GrowStructData(uint32_t size)
    {
        uint32_t nextStructPos = 0;
        uint32_t structDataSize = GetStructsDataSize();
        char* newStructData = static_cast<char*>(std::malloc(structDataSize + size));

        if (m_Structs)
        {
            std::memcpy(newStructData, m_Structs, structDataSize);
            std::free(m_Structs);
            m_Structs = newStructData;

            uint32_t structCount = GetNumStructs();
            structCount++;
            SetNumStructs(structCount);

            uint32_t structsSize = GetStructsDataSize();
            nextStructPos = structsSize;
            structsSize += size;
            SetStructsDataSize(structsSize);
        }
        else
        {
            m_Structs = newStructData;
            SetNumStructs(1);
            SetStructsDataSize(size);
            nextStructPos = s_StructsHeaderSize;
        }

        return nextStructPos;
    }

    uint32_t Database::GetObjectsDataSize()
    {
        if (!m_Objects)
        {
            return s_ObjectsHeaderSize; // enough room for required headers since this will be a new alloc
        }

        return *reinterpret_cast<uint32_t*>(m_Objects);
    }

    uint32_t Database::GrowObjectsData(uint32_t size)
    {
        uint32_t nextObjectPos = 0;
        uint32_t objectsDataSize = GetObjectsDataSize();
        char* newObjectsData = static_cast<char*>(std::malloc(objectsDataSize + size));

        if (m_Objects)
        {
            std::memcpy(newObjectsData, m_Objects, objectsDataSize);
            std::free(m_Objects);
            m_Objects = newObjectsData;
            nextObjectPos = objectsDataSize;
            SetObjectsDataSize(objectsDataSize + size);
        }
        else
        {
            m_Objects = newObjectsData;
            nextObjectPos = s_ObjectsHeaderSize;
            SetObjectsDataSize(size);
        }

        return nextObjectPos;
    }

    Struct Database::GetStruct(uint32_t structIndex)
    {
        uint32_t offset = 0;
        uint32_t structCount = GetNumStructs();
        if (!SPARK_VERIFY(structIndex < structCount))
        {
            return Struct(nullptr);
        }

        offset += s_StructsHeaderSize;
        char* structStart = m_Structs + offset;
        for (int i = 0; i < structIndex; ++i)
        {
            uint32_t structSize = GetNumStructFields(structStart);
            offset += (structSize * s_StructRowSize) + s_StructHeaderSize;
        }

        return Struct(m_Structs + offset);
    }

    void Database::Save(const std::string& path)
    {
        std::ofstream out;
        out.open (path, std::ios::binary);
        out.write(m_Structs, GetStructsDataSize());
        out.write(m_Objects, GetObjectsDataSize());
        out.write(m_Strings, GetStringsDataSize());
        out.close();
    }

    std::shared_ptr<Database> Database::Load(const std::string& path, bool readOnly)
    {
        if (readOnly)
        {
            char* data = io::FileSystem::ReadBinaryFile(path);

            std::shared_ptr<Database> asset = Create(true);
            asset->m_Structs = data;
            asset->m_Objects = asset->m_Structs + asset->GetStructsDataSize();
            asset->m_Strings = asset->m_Objects + asset->GetObjectsDataSize();
            return asset;
        }
        else
        {
            std::shared_ptr<Database> temp = Load(path, true);
            std::shared_ptr<Database> asset = Create(false);

            auto structsSize= temp->GetStructsDataSize();
            asset->m_Structs = static_cast<char*>(std::malloc(structsSize));
            std::memcpy(asset->m_Structs, temp->m_Structs, structsSize);

            auto objectsSize = temp->GetObjectsDataSize();
            asset->m_Objects = static_cast<char*>(std::malloc(objectsSize));
            std::memcpy(asset->m_Objects, temp->m_Objects, objectsSize);

            auto stringsSize = temp->GetStringsDataSize();
            asset->m_Strings = static_cast<char*>(std::malloc(stringsSize));
            std::memcpy(asset->m_Strings, temp->m_Strings, stringsSize);

            return asset;
        }
    }

    Object Database::CreateObject(uint32_t structIndex)
    {
        if (!SPARK_VERIFY(!m_ReadOnly))
        {
            return Object(nullptr, nullptr, Struct(nullptr));
        }

        Struct structDef = GetStruct(structIndex);
        uint32_t reqSize = structDef.CalcObjectSize();
        if (!m_Objects)
        {
            reqSize += s_ObjectsHeaderSize; // objectsDataSize (uint32_t)
        }
        auto offset = GrowObjectsData(reqSize);
        char* objData = m_Objects + offset;
        std::memset(objData, 0, reqSize);
        std::memcpy(objData, &structIndex, sizeof(uint32_t));

        return Object(m_Objects + offset, this, structDef);
    }

    void Database::SetRootStruct(uint32_t structIndex)
    {
        if (!SPARK_VERIFY(m_Objects == nullptr, "Changing root struct of Database not supported (yet)."))
        {
            return;
        }

        CreateObject(structIndex);
    }

    Object Database::GetRoot()
    {
        Struct structDef = GetStruct(*reinterpret_cast<uint32_t*>(m_Objects + sizeof(uint32_t)));
        return Object(m_Objects + sizeof(uint32_t), this, structDef);
    }

    uint32_t Database::GetNumStructs()
    {
        return *(reinterpret_cast<uint32_t*>(m_Structs));
    }

    void Database::SetNumStructs(uint32_t numStructs)
    {
        std::memcpy(m_Structs, &numStructs, sizeof(uint32_t));
    }

    void Database::SetStructsDataSize(uint32_t dataSize)
    {
        std::memcpy(m_Structs + sizeof(uint32_t), &dataSize, sizeof(uint32_t));
    }

    void Database::SetObjectsDataSize(uint32_t size)
    {
        std::memcpy(m_Objects, &size, sizeof(uint32_t));
    }

    uint32_t Database::GetNumStructFields(char* structData)
    {
        return *(reinterpret_cast<uint32_t*>(structData));
    }

    std::shared_ptr<Database> Database::Create(bool readOnly)
    {
        return std::shared_ptr<Database>(new Database(readOnly));
    }

    uint32_t Database::GetObjectOffset(const Object& obj)
    {
        SPARK_ASSERT(obj.m_DB == this);
        return obj.m_Data - m_Objects;
    }

    Object Database::GetObjectAt(uint32_t offset)
    {
        auto objData = m_Objects + offset;
        auto structIndex = *reinterpret_cast<uint32_t*>(objData);
        return Object(objData, this, GetStruct(structIndex));
    }

    uint32_t Database::GetStringsDataSize()
    {
        if (!m_Strings)
        {
            return s_StringsHeaderSize; // enough room for required headers since this will be a new alloc
        }

        return *reinterpret_cast<uint32_t*>(m_Strings);
    }

    void Database::SetStringsDataSize(uint32_t size)
    {
        std::memcpy(m_Strings, &size, sizeof(uint32_t));
    }

    uint32_t Database::GrowStringsData(uint32_t size)
    {
        uint32_t nextStringPos = 0;
        uint32_t stringsDataSize = GetStringsDataSize();
        char* newStringsData = static_cast<char*>(std::malloc(stringsDataSize + size));

        if (m_Strings)
        {
            std::memcpy(newStringsData, m_Strings, stringsDataSize);
            std::free(m_Strings);
            m_Strings = newStringsData;
            nextStringPos = stringsDataSize;
            SetStringsDataSize(stringsDataSize + size);
        }
        else
        {
            m_Strings = newStringsData;
            nextStringPos = s_StringsHeaderSize;
            SetStringsDataSize(size);
        }

        return nextStringPos;
    }

    uint32_t Database::CreateString(const std::string& str)
    {
        if (!SPARK_VERIFY(!m_ReadOnly))
        {
            return std::numeric_limits<uint32_t>().max();
        }

        uint32_t reqSize = str.length() + 1;
        if (!m_Strings)
        {
            reqSize += s_StringsHeaderSize;
        }
        auto offset = GrowStringsData(reqSize);
        char* stringData = m_Strings + offset;
        std::memcpy(stringData, str.data(), str.length());
        std::memset(stringData + str.length(), 0, 1);

        return stringData - m_Strings;
    }

    const char* Database::GetStringAt(uint32_t offset)
    {
        return m_Strings + offset;
    }
}