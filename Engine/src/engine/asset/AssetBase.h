#pragma once

namespace se::asset::binary
{
    class Database;
}

namespace se::asset
{
    class AssetBase
    {
    public:
        virtual ~AssetBase() = default;
        virtual std::shared_ptr<binary::Database> Serialise() = 0;
        virtual void Deserialise(const std::shared_ptr<binary::Database>& db) = 0;
    };
}
