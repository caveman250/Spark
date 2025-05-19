#include "engine/asset/builder/AssetBuilder.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        se::debug::Log::Error("No file supplied! aborting.");
        return -1;
    }
    if (argc < 3)
    {
        se::debug::Log::Error("No output path supplied! aborting.");
        return -1;
    }
    auto db = se::asset::builder::AssetBuilder::ProcessAsset(argv[1]);
    if (db)
    {
        db->Save(argv[2]);
    }
}
