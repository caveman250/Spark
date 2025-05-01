#pragma once
#include "spark.h"

namespace se::io
{
    struct VFSMount
    {
        String fsPath;
        String vfsPath;
    };
    
    struct VFSFile
    {
        String dir = {};
        String fileName = {};
        String extension = {};
        String fullPath = {};
        bool isDirectory = false;
    };

    class VFS
    {
    public:
        static VFS& Get();

        void Mount(const String& fsPath, const String& vfsPath);
        void Unmount(const String& vfsPath);
        bool Exists(const String& path);
        std::optional<String> ResolveFSPath(const String& vfsPath, bool allowMissing);
        std::time_t GetLastModified(const String& path);

        String ReadText(const String& path);
        void WriteText(const String& path, const String& text);
        char* ReadBinary(const String& path, size_t& outSize);
        void WriteBinary(const String& path, char* data, size_t size);

        void ForEachFile(const String& dirPath, bool recursive, const std::function<void(const VFSFile&)>& func);

        const std::vector<VFSMount>& GetMounts() const { return m_Mounts; }

    private:
#if SPARK_PLATFORM_WINDOWS
        static void FixWindowsPath(String& path);
#endif
        static bool IsMount(const String& path, const VFSMount& mount);
        String GetFSPath(const String& path, const VFSMount& mount);
        String GetVFSPath(const String& path);

        static VFS* s_Instance;

        std::vector<VFSMount> m_Mounts;
    };
}
