#include "engine/asset/binary/Database.h"
#include "engine/io/VFS.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        se::debug::Log::Error("No file supplied! aborting.");
        return -1;
    }

    std::string path = argv[1];
    auto db = se::asset::binary::Database::Load(path, true);
    if (db)
    {
        std::string newPath = path.replace(path.length() - 5, strlen(".sass"), ".json");
        auto json = db->ToJson().dump(4);

        se::io::VFS::Get().WriteText(newPath, json);
    }
}
