#include "VFS.h"

#include <filesystem>
#include "engine/string/util/StringUtil.h"
#include "engine/io/util/PathUtil.h"

namespace se::io
{
    VFS* VFS::s_Instance = nullptr;
    VFS& VFS::Get()
    {
        if (!s_Instance)
        {
            s_Instance = new VFS();
        }

        return *s_Instance;
    }

    void VFS::Mount(const String& fsPath, const String& vfsPath)
    {
        m_Mounts.push_back(VFSMount(fsPath, vfsPath));
    }

    void VFS::Unmount(const String& vfsPath)
    {
        auto it = std::find_if(m_Mounts.begin(), m_Mounts.end(),
                               [&vfsPath](const VFSMount& mount) { return mount.vfsPath == vfsPath; });
        if (it != m_Mounts.end())
        {
            m_Mounts.erase(it);
        }
    }

    String VFS::ReadText(const String& path)
    {
        auto fsPath = ResolveFSPath(path, false);
        if (!fsPath.has_value())
        {
            if (!path.StartsWith(".") && !path.StartsWith("/") && path[1] != ':')
            {
                debug::Log::Error("Cannot open file {0}!", path);
                return "";
            }

            fsPath = path;
        }

        String fileContents;
        std::ifstream fileStream(fsPath.value().Data(), std::ios::in);
        if (fileStream.is_open())
        {
            std::stringstream sstr;
            sstr << fileStream.rdbuf();
            fileContents = sstr.str();
            fileStream.close();
        }
        else
        {
            debug::Log::Error("Cannot read file {0}!", fsPath.value());
        }

        return fileContents;
    }

    char* VFS::ReadBinary(const String& path, size_t& outSize)
    {
        auto fsPath = ResolveFSPath(path, false);
        if (!fsPath.has_value())
        {
            if (!path.StartsWith(".") && !path.StartsWith("/") && path[1] != ':')
            {
                debug::Log::Error("Cannot open file {0}!", path);
                return nullptr;
            }

            fsPath = path;
        }

        std::ifstream myfile;
        myfile.open(fsPath.value().Data(), std::ios::binary | std::ios::ate);
        SPARK_ASSERT(myfile.is_open());
        std::streamsize size = myfile.tellg();
        myfile.seekg(0, std::ios::beg);
        char* data = static_cast<char*>(std::malloc(size));
        myfile.read(data, size);
        outSize = size;
        myfile.close();

        return data;
    }

    void VFS::WriteBinary(const String& path, char* data, size_t size)
    {
        auto fsPath = ResolveFSPath(path, true);
        if (!fsPath.has_value())
        {
            if (!path.StartsWith(".") && !path.StartsWith("/") && path[1] != ':')
            {
                debug::Log::Error("Cannot open file {0}!", path);
                return;
            }

            fsPath = path;
        }

        std::ofstream myfile;
        myfile.open(fsPath.value().Data(), std::ios::binary);
        myfile.write(data, size);
        myfile.close();
    }

    std::optional<String> VFS::ResolveFSPath(const String& vfsPath, bool allowMissing)
    {
        for (const auto& mount : m_Mounts)
        {
            if (IsMount(vfsPath, mount))
            {
                auto workingDir = std::filesystem::current_path();
                auto fsPath = GetFSPath(vfsPath, mount);
                if (std::filesystem::exists(fsPath.Data()))
                {
                    return fsPath;
                }
            }
        }

        if (allowMissing)
        {
            for (const auto& mount : m_Mounts)
            {
                if (IsMount(vfsPath, mount))
                {
                    auto fsPath = GetFSPath(vfsPath, mount);
                    std::filesystem::create_directories(fsPath.SubString(0, fsPath.Find('/', true)).Data());
                    return fsPath;
                }
            }
        }

        return std::nullopt;
    }

    bool VFS::IsMount(const String& path, const VFSMount& mount)
    {
        if (!path.StartsWith(mount.vfsPath))
        {
            return false;
        }

        return mount.vfsPath[mount.vfsPath.Size() - 1] == '/' ||
            path.Size() == mount.vfsPath.Size() ||
            path[mount.vfsPath.Size()] == '/';
    }

    String VFS::GetFSPath(const String& path, const VFSMount& mount)
    {
        if (path == mount.vfsPath)
        {
            return mount.fsPath;
        }
        else
        {
            return mount.fsPath + '/' + String(path.Data() + mount.vfsPath.Size() + 1);
        }
    }

    void VFS::WriteText(const String& path, const String& text)
    {
        auto fsPath = ResolveFSPath(path, true);
        if (!fsPath.has_value())
        {
            if (!path.StartsWith(".") && !path.StartsWith("/") && path[1] != ':')
            {
                debug::Log::Error("Cannot open file {0}!", path);
                return;
            }

            fsPath = path;
        }

        std::ofstream myfile;
        myfile.open(fsPath.value().Data());
        myfile << text;
        myfile.close();
    }

    bool VFS::Exists(const String& path)
    {
        return ResolveFSPath(path, false).has_value();
    }

    void VFS::ForEachFile(const String& dirPath, bool recursive, const std::function<void(const VFSFile&)>& func)
    {
        auto fsPath = ResolveFSPath(dirPath, false);
        if (!fsPath.has_value())
        {
            return;
        }

        if (recursive)
        {
            for (const std::filesystem::directory_entry& file: std::filesystem::recursive_directory_iterator(fsPath.value().Data()))
            {
                VFSFile vfsFile = {
                    .fullPath = GetVFSPath(file.path().string()),
                    .isDirectory = file.is_directory()
                };
                io::util::SplitPath(vfsFile.fullPath, vfsFile.dir, vfsFile.fileName, vfsFile.extension);
                func(vfsFile);
            }
        }
        else
        {
            for (const std::filesystem::directory_entry& file: std::filesystem::directory_iterator(fsPath.value().Data()))
            {
                VFSFile vfsFile = {
                        .fullPath = GetVFSPath(file.path().string()),
                        .isDirectory = file.is_directory()
                };
                io::util::SplitPath(vfsFile.fullPath, vfsFile.dir, vfsFile.fileName, vfsFile.extension);
                func(vfsFile);
            }
        }
    }

    String VFS::GetVFSPath(const String& path)
    {
        for (const auto& mount : m_Mounts)
        {
            if (path.Contains(mount.fsPath))
            {
                String temp = path;
                String dir, fileName;
                string::util::Split(temp, dir, fileName, '/', true);
#if SPARK_PLATFORM_WINDOWS
                FixWindowsPath(temp);
#endif
                return mount.vfsPath + String(temp.Data() + mount.fsPath.Size());
            }
        }

        return "";
    }

#if SPARK_PLATFORM_WINDOWS
    void VFS::FixWindowsPath(String& path)
    {
        std::replace( path.begin(), path.end(), '\\', '/' );
    }
#endif

    template <typename TP>
    std::time_t to_time_t(TP tp)
    {
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
                  + system_clock::now());
        return system_clock::to_time_t(sctp);
    }

    std::time_t VFS::GetLastModified(const String& path)
    {
        auto result = ResolveFSPath(path, false);
        if (result.has_value())
        {
            std::chrono::time_point<std::chrono::file_clock> timePoint = std::filesystem::last_write_time(result.value().Data());
            return to_time_t(timePoint);
        }

        return 0;
    }
}
