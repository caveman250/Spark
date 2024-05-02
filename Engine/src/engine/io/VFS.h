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
        bool Exists(const std::string& path);
        std::optional<std::string> ResolveFSPath(const std::string& vfsPath, bool allowMissing);
        std::time_t GetLastModified(const std::string path);

        std::string ReadText(const std::string& path);
        void WriteText(const std::string& path, const std::string& text);
        char* ReadBinary(const std::string& path, size_t& outSize);
        void WriteBinary(const std::string& path, char* data, size_t size);

        void ForEachFile(const std::string& dirPath, bool recursive, const std::function<void(const std::string&)>& func);

    private:
#if SPARK_PLATFORM_WINDOWS
    static void FixPath(std::string& path);
#endif
        static bool IsMount(const std::string& path, const VFSMount& mount);
        std::string GetFSPath(const std::string& path, const VFSMount& mount);
        std::string GetVFSPath(const std::string& path);

        static VFS* s_Instance;

        std::vector<VFSMount> m_Mounts;
    };
}
