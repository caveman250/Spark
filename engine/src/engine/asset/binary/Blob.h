#pragma once
#include "spark.h"

namespace se::asset::binary
{
    constexpr uint32_t s_BlobHeaderSize = sizeof(uint32_t); // size (uint32_t)

    class Database;
    class Blob
    {
    public:
        const char* GetData() { return m_Data + s_BlobHeaderSize; }
        uint32_t GetSize() { return *reinterpret_cast<uint32_t*>(m_Data); }

    private:
        Blob(char* data, Database* db) : m_Data(data), m_DB(db) {}
        char* m_Data;
        [[maybe_unused]] Database* m_DB;

        friend class Database;
    };
}
