#include <engine/io/VFS.h>
#include "Database.h"

#include "Array.h"
#include "engine/io/OutputFileStream.h"

namespace se::asset::binary
{
    constexpr uint32_t s_StructsHeaderSize = sizeof(uint32_t) * 2; // numStructs (uint32_t), totalStructsSize (uint32_t)]
    constexpr uint32_t s_ObjectsHeaderSize = sizeof(uint32_t); // totalSize (uint32_t)
    constexpr uint32_t s_StringsHeaderSize = sizeof(uint32_t); // totalSize (uint32_t)
    constexpr uint32_t s_BlobsHeaderSize = sizeof(uint32_t); // totalSize (uint32_t)

    Database::Database(bool readOnly)
            : m_ReadOnly(readOnly)
    {
    }

    Database::~Database()
    {
        if (m_ReadOnly)
        {
            if (m_Structs)
            {
                std::free(m_Structs); // all one allocation
            }
        }
        else
        {
            if (m_Structs)
            {
                std::free(m_Structs);
            }

            if (m_Objects)
            {
                free(m_Objects);
            }

            if (m_Strings)
            {
                std::free(m_Strings);
            }
        }
    }

    void Database::CreateStructData(const std::string& name, const std::vector<std::pair<FixedString32, Type>>& structLayout, void* createAt)
    {
        char* structData = static_cast<char*>(createAt);

        size_t structSize = structLayout.size();
        std::memcpy(structData, &structSize, sizeof(uint32_t));
        FixedString32 fixedName = CreateFixedString32(name.c_str());
        std::memcpy(structData + sizeof(uint32_t), &fixedName, 32);
        uint32_t offset = s_StructHeaderSize;

        uint32_t rowSize = sizeof(Type) + 32;
        for (const auto& [name, type] : structLayout)
        {
            std::memcpy(structData + offset, &name, 32);
            std::memcpy(structData + offset + 32, &type, sizeof(Type));
            offset += rowSize;
        }
    }

    uint32_t Database::CalcStructDefinitionDataSize(const std::vector<std::pair<FixedString32, Type>>& structLayout)
    {
        return s_StructRowSize * static_cast<uint32_t>(structLayout.size()) + s_StructHeaderSize;
    }

    uint32_t Database::GetOrCreateStruct(const std::string& name, const StructLayout& structLayout)
    {
        if (!SPARK_VERIFY(!m_ReadOnly))
        {
            return std::numeric_limits<uint32_t>().max();
        }

        if (m_StructCache.contains(name))
        {
            return m_StructCache.at(name);
        }

        uint32_t reqSize = CalcStructDefinitionDataSize(structLayout);
        uint32_t pos = GrowStructData(reqSize);
        CreateStructData(name, structLayout, m_Structs + pos);

        uint32_t structIndex = GetNumStructs() - 1;
        m_StructCache[name] = structIndex;
        m_StructNameCache[structIndex] = name;
        return structIndex;
    }

    std::string Database::GetStructName(uint32_t structIndex)
    {
        if (!m_StructNameCache.contains(structIndex))
        {
            std::string name = std::string(GetStructData(structIndex) + s_StructHeaderSize, 32);
            m_StructNameCache[structIndex] = name;
            m_StructCache[name] = structIndex;
            return name;
        }

        return m_StructNameCache.at(structIndex);
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

            nextStructPos = structDataSize;
        }
        else
        {
            m_Structs = newStructData;
            SetNumStructs(1);
            nextStructPos = s_StructsHeaderSize;
        }

        SetStructsDataSize(structDataSize + size);

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
        }
        else
        {
            m_Objects = newObjectsData;
            nextObjectPos = s_ObjectsHeaderSize;
        }

        SetObjectsDataSize(objectsDataSize + size);

        return nextObjectPos;
    }

    char* Database::GetStructData(uint32_t structIndex)
    {
        uint32_t offset = 0;
        uint32_t structCount = GetNumStructs();
        if (!SPARK_VERIFY(structIndex < structCount))
        {
            return nullptr;
        }

        offset += s_StructsHeaderSize;
        for (uint8_t i = 0; i < structIndex; ++i)
        {
            uint32_t structSize = GetNumStructFields(m_Structs + offset);
            offset += (structSize * s_StructRowSize) + s_StructHeaderSize;
        }

        return m_Structs + offset;
    }

    void Database::Save(const std::string& path)
    {
        io::OutputFileStream stream(path, true);

        if (!m_Structs)
        {
            GrowStructData(0); // ensure minimum viable info exists
        }
        stream.WriteBinary(m_Structs, GetStructsDataSize());

        if (!m_Objects)
        {
            GrowObjectsData(0); // ensure minimum viable info exists
        }
        stream.WriteBinary(m_Objects, GetObjectsDataSize());

        if (!m_Strings)
        {
            GrowStringsData(0); // ensure minimum viable info exists
        }
        stream.WriteBinary(m_Strings, GetStringsDataSize());

        if (!m_BlobData)
        {
            GrowBlobData(0); // ensure minimum viable info exists
        }
        stream.WriteBinary(m_BlobData, GetBlobDataSize());
        stream.Close();
    }

    nlohmann::ordered_json Database::ToJson()
    {
        nlohmann::ordered_json ret;

        nlohmann::ordered_json structs;
        for (uint32_t i = 0; i < GetNumStructs(); ++i)
        {
            auto name = GetStructName(i);
            auto dbStruct = Struct(name, i, this);

            nlohmann::ordered_json structJson;
            structJson[name] = dbStruct.ToJson();
            structs.push_back(structJson);
        }

        ret["structs"] = structs;
        ret["root"] = GetRoot().ToJson();

        return ret;
    }

    std::shared_ptr<Database> Database::Load(const std::string& path, bool readOnly)
    {
        if (readOnly)
        {
            size_t size;
            char* data = io::VFS::Get().ReadBinary(path, size);

            std::shared_ptr<Database> asset = Create(true);
            asset->m_Structs = data;
            asset->m_Objects = asset->m_Structs + asset->GetStructsDataSize();
            asset->m_Strings = asset->m_Objects + asset->GetObjectsDataSize();
            asset->m_BlobData = asset->m_Strings + asset->GetStringsDataSize();

            asset->CacheStructs();

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

            auto blobsSize = temp->GetBlobDataSize();
            asset->m_BlobData = static_cast<char*>(std::malloc(blobsSize));
            std::memcpy(asset->m_BlobData, temp->m_BlobData, blobsSize);

            asset->CacheStructs();

            return asset;
        }
    }

    Object Database::CreateObject(uint32_t structIndex)
    {
        if (!SPARK_VERIFY(!m_ReadOnly))
        {
            return Object(std::numeric_limits<uint32_t>().max(), nullptr, Struct::Invalid);
        }

        Struct structDef = Struct(m_StructNameCache.at(structIndex), structIndex, this);
        uint32_t reqSize = structDef.CalcObjectSize();
        if (!m_Objects)
        {
            reqSize += s_ObjectsHeaderSize; // objectsDataSize (uint32_t)
        }
        auto offset = GrowObjectsData(reqSize);
        char* objData = m_Objects + offset;
        std::memset(objData, 0, reqSize);
        std::memcpy(objData, &structIndex, sizeof(uint32_t));

        return Object(offset, this, structDef);
    }

    void Database::SetRootStruct(uint32_t structIndex)
    {
        if (!SPARK_VERIFY(m_Objects == nullptr, "Changing root struct of Database not supported (yet)."))
        {
            return;
        }

        CreateObject(structIndex);
    }

    void Database::CacheStructs()
    {
        for (uint32_t i = 0; i < GetNumStructs(); ++i)
        {
            auto name = GetStructName(i);
            m_StructCache[name] = i;
            m_StructNameCache[i] = name;
        }
    }

    Object Database::GetRoot()
    {
        // assuming root struct is first struct
        Struct structDef = Struct(GetStructName(0), 0, this);
        return Object(sizeof(uint32_t), this, structDef);
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

    Object Database::GetObjectAt(uint32_t offset)
    {
        auto objData = GetObjectDataAt(offset);
        auto structIndex = *reinterpret_cast<uint32_t*>(objData);
        return Object(offset, this, Struct(m_StructNameCache.at(structIndex), structIndex, this));
    }

    char* Database::GetObjectDataAt(uint32_t offset) const
    {
        return m_Objects + offset;
    }

    Array Database::CreateArray(uint32_t structIndex, size_t count)
    {
        if (!SPARK_VERIFY(!m_ReadOnly))
        {
            return Array(std::numeric_limits<uint32_t>().max(), nullptr, false);
        }

        Struct structDef = Struct(m_StructNameCache.at(structIndex), structIndex, this);
        uint32_t reqSize = s_ArrayHeaderSize + structDef.CalcObjectSize() * (uint32_t)count;
        if (!m_Objects)
        {
            reqSize += s_ObjectsHeaderSize;
        }
        auto offset = GrowObjectsData(reqSize);
        char* objData = m_Objects + offset;
        std::memset(objData, 0, reqSize);
        std::memcpy(objData, &count, sizeof(uint32_t));
        std::memcpy(objData + sizeof(uint32_t), &structIndex, sizeof(uint32_t));

        return Array(offset, this, true);
    }

    Array Database::GetArrayAt(uint32_t offset)
    {
        return Array(offset, this, false);
    }

    PolymorphicArray Database::GetPolymorphicArrayAt(uint32_t offset)
    {
        return PolymorphicArray(offset, this);
    }

    PolymorphicArray Database::CreatePolymorphicArray(size_t count)
    {
        if (!SPARK_VERIFY(!m_ReadOnly))
        {
            return PolymorphicArray(std::numeric_limits<uint32_t>().max(), nullptr);
        }

        static StructLayout structLayout = {
            { CreateFixedString32("valid"), Type::Bool },
            { CreateFixedString32("offset"), Type::Uint32 }
        };

        auto structIndex = GetOrCreateStruct("Internal_ArrayItem", structLayout);
        Struct structDef = Struct(m_StructNameCache.at(structIndex), structIndex, this);
        uint32_t reqSize = s_ArrayHeaderSize + structDef.CalcObjectSize() * (uint32_t)count;
        if (!m_Objects)
        {
            reqSize += s_ObjectsHeaderSize;
        }
        auto offset = GrowObjectsData(reqSize);
        char* objData = m_Objects + offset;
        std::memset(objData, 0, reqSize);
        std::memcpy(objData, &count, sizeof(uint32_t));
        std::memcpy(objData + sizeof(uint32_t), &structIndex, sizeof(uint32_t));

        return PolymorphicArray(offset, this);
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
        }
        else
        {
            m_Strings = newStringsData;
            nextStringPos = s_StringsHeaderSize;
        }

        SetStringsDataSize(stringsDataSize + size);

        return nextStringPos;
    }

    uint32_t Database::CreateString(const std::string& str)
    {
        if (!SPARK_VERIFY(!m_ReadOnly))
        {
            return std::numeric_limits<uint32_t>().max();
        }

        uint32_t reqSize = static_cast<uint32_t>(str.length()) + 1u;
        if (!m_Strings)
        {
            reqSize += s_StringsHeaderSize;
        }
        auto offset = GrowStringsData(reqSize);
        char* stringData = m_Strings + offset;
        std::memcpy(stringData, str.data(), str.length());
        std::memset(stringData + str.length(), 0, 1);

        return static_cast<uint32_t>(stringData - m_Strings);
    }

    const char* Database::GetStringAt(uint32_t offset)
    {
        return m_Strings + offset;
    }

    uint32_t Database::GetBlobDataSize()
    {
        if (!m_BlobData)
        {
            return s_BlobsHeaderSize; // enough room for required headers since this will be a new alloc
        }

        return *reinterpret_cast<uint32_t*>(m_BlobData);
    }

    void Database::SetBlobDataSize(uint32_t size)
    {
        std::memcpy(m_BlobData, &size, sizeof(uint32_t));
    }

    uint32_t Database::GrowBlobData(uint32_t size)
    {
        uint32_t nextBlobPos = 0;
        uint32_t blobDataSize = GetBlobDataSize();
        char* newBlobData = static_cast<char*>(std::malloc(blobDataSize + size));

        if (m_BlobData)
        {
            std::memcpy(newBlobData, m_BlobData, blobDataSize);
            std::free(m_BlobData);
            m_BlobData = newBlobData;
            nextBlobPos = blobDataSize;
        }
        else
        {
            m_BlobData = newBlobData;
            nextBlobPos = s_BlobsHeaderSize;
        }

        SetBlobDataSize(blobDataSize + size);

        return nextBlobPos;
    }

    Blob Database::CreateBlob(const char* data, uint32_t size)
    {
        if (!SPARK_VERIFY(!m_ReadOnly))
        {
            return Blob(nullptr, nullptr);
        }

        uint32_t reqSize = size;
        if (!m_BlobData)
        {
            reqSize += s_BlobsHeaderSize; // objectsDataSize (uint32_t)
        }
        auto offset = GrowBlobData(reqSize);
        char* blobData = m_BlobData + offset;
        std::memcpy(blobData, &size, sizeof(uint32_t));
        std::memcpy(blobData + s_BlobHeaderSize, data, size);

        return Blob(blobData, this);
    }

    uint32_t Database::GetBlobOffset(const Blob& blob)
    {
        SPARK_ASSERT(blob.m_DB == this);
        return static_cast<uint32_t>(blob.m_Data - m_BlobData);
    }

    Blob Database::GetBlobAt(uint32_t offset)
    {
        auto blobData = m_BlobData + offset;
        return Blob(blobData, this);
    }
}
