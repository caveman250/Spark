#pragma once

#include "Struct.h"
#include "binary.h"
#include "Object.h"
#include "Array.h"
#include "Blob.h"

namespace se::asset::binary
{
    class Database
    {
    public:
        static std::shared_ptr<Database> Create(bool readOnly);
        ~Database();
        [[nodiscard]] uint32_t CreateStruct(const StructLayout& structLayout);
        char* GetStructData(uint32_t structIndex);

        Object CreateObject(uint32_t structIndex);
        Object GetObjectAt(uint32_t offset);
        char* GetObjectDataAt(uint32_t offset) const;

        Array CreateArray(uint32_t structIndex, uint32_t count);
        Array GetArrayAt(uint32_t offset);

        uint32_t CreateString(const std::string& str);
        const char* GetStringAt(uint32_t offset);

        Blob CreateBlob(const char* data, uint32_t size);
        uint32_t GetBlobOffset(const Blob& blob);
        Blob GetBlobAt(uint32_t offset);

        Object GetRoot();
        void SetRootStruct(uint32_t structIndex);

        static std::shared_ptr<Database> Load(const std::string& path, bool readOnly);
        void Save(const std::string& path);

    private:
        explicit Database(bool readOnly);

        static void CreateStructData(const std::vector<std::pair<FixedString32, Type>>& structLayout, void* createAt);
        static uint32_t CalcStructDefinitionDataSize(const std::vector<std::pair<FixedString32, Type>>& structLayout);

        uint32_t GetStructsDataSize();
        void SetStructsDataSize(uint32_t dataSize);
        uint32_t GrowStructData(uint32_t minSize);
        void SetNumStructs(uint32_t numStructs);
        uint32_t GetNumStructs();

        uint32_t GetNumStructFields(char* structData);

        uint32_t GetObjectsDataSize();
        void SetObjectsDataSize(uint32_t size);
        uint32_t GrowObjectsData(uint32_t minSize);

        uint32_t GetStringsDataSize();
        void SetStringsDataSize(uint32_t size);
        uint32_t GrowStringsData(uint32_t size);

        uint32_t GetBlobDataSize();
        void SetBlobDataSize(uint32_t size);
        uint32_t GrowBlobData(uint32_t size);

        bool m_ReadOnly = true;

        char* m_Structs = nullptr;
        // numStructs (uint32_t)
        // totalStructsSize (uint32_t)
        // struct1Count (uint32_t)
        // struct1
        //...
        // structNCount (uint32_t)
        // structN

        char* m_Objects = nullptr;
        // Objects size (uint32_t)
        // root struct index (uint32_t)
        // root object

        char* m_Strings = nullptr;
        // Strings size (uint32_t)
        // string1Size (uint32_t)
        // string1
        //...
        // stringNSize (uint32_t)
        // stringN

        char* m_BlobData = nullptr;
        // blobs size (uint32_t)
        // blob1Size (uint32_t)
        // blob1
        //...
        // blobNSize (uint32_t)
        // blobN
    };
}
