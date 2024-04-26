#include <engine/io/FileSystem.h>
#include "Asset.h"

namespace se::asset::binary
{
    constexpr uint32_t s_StructsHeaderSize = sizeof(uint32_t) * 2; // numStructs (uint32_t), totalStructsSize (uint32_t)]
    constexpr uint32_t s_ObjectsHeaderSize = sizeof(uint32_t); // totalSize (uint32_t)

    Asset::Asset(bool readOnly)
            : m_ReadOnly(readOnly)
    {

    }

    Asset::~Asset()
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

    void Asset::CreateStructData(const std::vector<std::pair<FixedString32, Type>>& structLayout, void* createAt)
    {
        char* structData = static_cast<char*>(createAt);

        uint32_t structSize = structLayout.size();
        std::memcpy(structData, &structSize, sizeof(uint32_t));

        uint32_t offset = sizeof(uint32_t);
        int index = 0;
        uint32_t rowSize = sizeof(Type) + 32;
        for (const auto& [name, type] : structLayout)
        {
            std::memcpy(structData + offset, &name, 32);
            std::memcpy(structData + offset + 32, &type, sizeof(Type));
            index++;
            offset += rowSize * index;
        }
    }

    uint32_t Asset::CalcStructDefinitionDataSize(const std::vector<std::pair<FixedString32, Type>>& structLayout)
    {
        return s_StructRowSize * structLayout.size() + s_StructHeaderSize;
    }

    uint32_t Asset::AddStruct(const StructLayout& structLayout)
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

    uint32_t Asset::GetStructsDataSize()
    {
        if (!m_Structs)
        {
            return s_StructsHeaderSize;
        }

        return *(reinterpret_cast<uint32_t*>(m_Structs) + 1);
    }

    uint32_t Asset::GrowStructData(uint32_t size)
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

    uint32_t Asset::GetObjectsDataSize()
    {
        if (!m_Objects)
        {
            return sizeof(uint32_t); // enough room for required headers since this will be a new alloc
        }

        return *reinterpret_cast<uint32_t*>(m_Objects);
    }

    uint32_t Asset::GrowObjectsData(uint32_t size)
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

    Struct Asset::GetStruct(uint32_t structIndex)
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

    void Asset::Save(const std::string& path)
    {
       // io::FileSystem::WriteBinaryFile(path, m_Structs, GetStructsDataSize());

        std::ofstream myfile;
        myfile.open (path, std::ios::binary);
        myfile.write(m_Structs, GetStructsDataSize());
        myfile.write(m_Objects, GetObjectsDataSize());
        myfile.close();
    }

    std::shared_ptr<Asset> Asset::Load(const std::string& path, bool readOnly)
    {
        if (readOnly)
        {
            char* data = io::FileSystem::ReadBinaryFile(path);

            std::shared_ptr<Asset> asset = Create(true);
            asset->m_Structs = data;
            asset->m_Objects = data + asset->GetStructsDataSize();
            return asset;
        }
        else
        {
            std::shared_ptr<Asset> temp = Load(path, true);
            std::shared_ptr<Asset> asset = Create(false);

            auto structsSize= temp->GetStructsDataSize();
            asset->m_Structs = static_cast<char*>(std::malloc(structsSize));
            std::memcpy(asset->m_Structs, temp->m_Structs, structsSize);

            auto objectsSize= temp->GetObjectsDataSize();
            asset->m_Objects = static_cast<char*>(std::malloc(objectsSize));
            std::memcpy(asset->m_Objects, temp->m_Objects, objectsSize);
            return asset;
        }
    }

    Object Asset::AddObject(uint32_t structIndex)
    {
        if (!SPARK_VERIFY(!m_ReadOnly))
        {
            return Object(nullptr, Struct(nullptr));
        }

        Struct structDef = GetStruct(structIndex);
        uint32_t reqSize = structDef.CalcObjectSize();
        if (!m_Objects)
        {
            reqSize += sizeof(uint32_t); // objectsDataSize (uint32_t)
        }
        auto offset = GrowObjectsData(reqSize);
        char* objData = m_Objects + offset;
        std::memset(objData, 0, reqSize);
        std::memcpy(objData, &structIndex, sizeof(uint32_t));

        auto totalObjSize = *reinterpret_cast<uint32_t*>(m_Objects);
        auto objSize = structDef.CalcObjectSize();
        totalObjSize += objSize;
        std::memcpy(m_Objects, &totalObjSize, sizeof(uint32_t));


        return Object(m_Objects + offset, structDef);
    }

    void Asset::SetRootStruct(uint32_t structIndex)
    {
        if (!SPARK_VERIFY(m_Objects == nullptr, "Changing root struct of Asset not supported (yet)."))
        {
            return;
        }

        AddObject(structIndex);
    }

    Object Asset::GetRoot()
    {
        Struct structDef = GetStruct(*reinterpret_cast<uint32_t*>(m_Objects + sizeof(uint32_t)));
        return Object(m_Objects + sizeof(uint32_t), structDef);
    }

    uint32_t Asset::GetNumStructs()
    {
        return *(reinterpret_cast<uint32_t*>(m_Structs));
    }

    void Asset::SetNumStructs(uint32_t numStructs)
    {
        std::memcpy(m_Structs, &numStructs, sizeof(uint32_t));
    }

    void Asset::SetStructsDataSize(uint32_t dataSize)
    {
        std::memcpy(m_Structs + sizeof(uint32_t), &dataSize, sizeof(uint32_t));
    }

    void Asset::SetObjectsDataSize(uint32_t size)
    {
        std::memcpy(m_Objects, &size, sizeof(uint32_t));
    }

    uint32_t Asset::GetNumStructFields(char* structData)
    {
        return *(reinterpret_cast<uint32_t*>(structData));
    }

    std::shared_ptr<Asset> Asset::Create(bool readOnly)
    {
        return std::shared_ptr<Asset>(new Asset(readOnly));
    }

}