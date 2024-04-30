#pragma once
#include "spark.h"

namespace se::io
{
    struct VFSMount
    {
        std::string fsPath;
        std::string vfsPath;
    };

    class VFS
    {
    public:
        static VFS& Get();

        void Mount(const std::string& fsPath, const std::string& vfsPath);
        std::optional<std::string> ResolveFSPath(const std::string& vfsPath, bool allowMissing);

        std::string ReadText(const std::string& path);
        char* ReadBinary(const std::string& path, size_t& outSize);
        void WriteBinary(const std::string& path, char* data, size_t size);

    private:
        static bool IsMount(const std::string& path, const VFSMount& mount);
        std::string GetFSPath(const std::string& path, const VFSMount& mount);

        static VFS* s_Instance;

        std::vector<VFSMount> m_Mounts;
    };
}
