#include "VFS.h"

#include <filesystem>

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

    void VFS::Mount(const std::string& fsPath, const std::string& vfsPath)
    {
        m_Mounts.push_back(VFSMount(fsPath, vfsPath));
    }

    std::string VFS::ReadText(const std::string& path)
    {
        const auto fsPath = ResolveFSPath(path, false);
        if (!fsPath.has_value())
        {
            debug::Log::Error("Cannot open file {0}!", path);
            return "";
        }

        std::string fileContents;
        std::ifstream fileStream(fsPath.value(), std::ios::in);
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

    char* VFS::ReadBinary(const std::string& path, size_t& outSize)
    {
        const auto fsPath = ResolveFSPath(path, false);
        if (!fsPath.has_value())
        {
            debug::Log::Error("Cannot open file {0}!", path);
            return nullptr;
        }

        std::ifstream myfile;
        myfile.open(fsPath.value(), std::ios::binary | std::ios::ate);
        std::streamsize size = myfile.tellg();
        myfile.seekg(0, std::ios::beg);
        char* data = static_cast<char*>(std::malloc(size));
        myfile.read(data, size);
        outSize = size;
        myfile.close();

        return data;
    }

    void VFS::WriteBinary(const std::string& path, char* data, size_t size)
    {
        const auto fsPath = ResolveFSPath(path, true);
        if (!fsPath.has_value())
        {
            debug::Log::Error("Cannot open file {0}!", path);
            return;
        }

        std::ofstream myfile;
        myfile.open(fsPath.value(), std::ios::binary);
        myfile.write(data, size);
        myfile.close();
    }

    std::optional<std::string> VFS::ResolveFSPath(const std::string& vfsPath, bool allowMissing)
    {
        for (const auto& mount : m_Mounts)
        {
            if (IsMount(vfsPath, mount))
            {
                auto fsPath = GetFSPath(vfsPath, mount);
                if (std::filesystem::exists(fsPath))
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
                    std::filesystem::create_directories(fsPath.substr(0, fsPath.find_last_of('/')));
                    return fsPath;
                }
            }
        }

        return std::nullopt;
    }

    bool VFS::IsMount(const std::string& path, const VFSMount& mount)
    {
        if (!path.starts_with(mount.vfsPath))
        {
            return false;
        }

        return mount.vfsPath.back() == '/' ||
            path.size() == mount.vfsPath.size() ||
            path[mount.vfsPath.size()] == '/';
    }

    std::string VFS::GetFSPath(const std::string& path, const VFSMount& mount)
    {
        if (path == mount.vfsPath)
        {
            return mount.fsPath;
        }
        else
        {
            return mount.fsPath + std::string(path.begin() + mount.vfsPath.size(),
                                                path.begin() + path.size());
        }
    }

    void VFS::WriteText(const std::string& path, const std::string& text)
    {
        const auto fsPath = ResolveFSPath(path, true);
        if (!fsPath.has_value())
        {
            debug::Log::Error("Cannot open file {0}!", path);
            return;
        }

        std::ofstream myfile;
        myfile.open(fsPath.value());
        myfile << text;
        myfile.close();
    }

    bool VFS::Exists(const std::string& path)
    {
        return ResolveFSPath(path, false).has_value();
    }

    void VFS::ForEachFile(const std::string& dirPath, bool recursive, const std::function<void(const std::string&)>& func)
    {
        auto fsPath = ResolveFSPath(dirPath, false);
        if (!fsPath.has_value())
        {
            return;
        }

        if (recursive)
        {
            for (const std::filesystem::directory_entry& file: std::filesystem::recursive_directory_iterator(fsPath.value()))
            {
                if (!file.is_directory())
                {
                    func(GetVFSPath(file.path().string()));
                }
            }
        }
        else
        {
            for (const std::filesystem::directory_entry& file: std::filesystem::directory_iterator(fsPath.value()))
            {
                if (!file.is_directory())
                {
                    func(GetVFSPath(file.path().string()));
                }
            }
        }
    }

    std::string VFS::GetVFSPath(const std::string& path)
    {
        for (const auto& mount : m_Mounts)
        {
            if (path.contains(mount.fsPath))
            {
                std::string temp = path;
#if SPARK_PLATFORM_WINDOWS
                FixWindowsPath(temp);
#endif
                return mount.vfsPath + std::string(temp.begin() + mount.fsPath.size(),
                                                   temp.begin() + temp.size());
            }
        }

        return "";
    }

#if SPARK_PLATFORM_WINDOWS
    void VFS::FixWindowsPath(std::string& path)
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

    std::time_t VFS::GetLastModified(const std::string path)
    {
        auto result = ResolveFSPath(path, false);
        if (result.has_value())
        {
            std::chrono::time_point<std::chrono::file_clock> timePoint = std::filesystem::last_write_time(result.value());
            return to_time_t(timePoint);
        }

        return 0;
    }
}
