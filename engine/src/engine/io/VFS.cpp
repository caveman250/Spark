#include "VFS.h"

#include <filesystem>
#include "engine/string/util/StringUtil.h"
#include "engine/io/util/PathUtil.h"
#include "engine/threads/ParallelForEach.h"

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

    void VFS::Mount(const std::string& fsPath, const std::string& vfsPath, bool visibleInEditor)
    {
        m_Mounts.push_back(VFSMount(fsPath, vfsPath, visibleInEditor));
    }

    void VFS::Unmount(const std::string& vfsPath)
    {
        auto it = std::find_if(m_Mounts.begin(), m_Mounts.end(),
                               [&vfsPath](const VFSMount& mount) { return mount.vfsPath == vfsPath; });
        if (it != m_Mounts.end())
        {
            m_Mounts.erase(it);
        }
    }

    std::string VFS::ReadText(const std::string& path)
    {
        auto fsPath = ResolveFSPath(path, false);
        if (!fsPath.has_value())
        {
            if (!path.starts_with(".") && !path.starts_with("/") && path[1] != ':')
            {
                debug::Log::Error("Cannot open file {0}!", path);
                return "";
            }

            fsPath = path;
        }

        std::string fileContents;
        std::ifstream fileStream(fsPath.value().data(), std::ios::in);
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
        auto fsPath = ResolveFSPath(path, false);
        if (!fsPath.has_value())
        {
            if (!path.starts_with(".") && !path.starts_with("/") && path[1] != ':')
            {
                debug::Log::Error("Cannot open file {0}!", path);
                return nullptr;
            }

            fsPath = path;
        }

        std::ifstream myfile;
        myfile.open(fsPath.value().data(), std::ios::binary | std::ios::ate);
        SPARK_ASSERT(myfile.is_open());
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
        auto fsPath = ResolveFSPath(path, true);
        if (!fsPath.has_value())
        {
            if (!path.starts_with(".") && !path.starts_with("/") && path[1] != ':')
            {
                debug::Log::Error("Cannot open file {0}!", path);
                return;
            }

            fsPath = path;
        }

        std::ofstream myfile;
        myfile.open(fsPath.value().data(), std::ios::binary);
        myfile.write(data, size);
        myfile.close();
    }

    std::optional<std::string> VFS::ResolveFSPath(const std::string& vfsPath, bool allowMissing)
    {
        for (const auto& mount : m_Mounts)
        {
            if (IsMount(vfsPath, mount))
            {
                auto workingDir = std::filesystem::current_path();
                auto fsPath = GetFSPath(vfsPath, mount);
                if (std::filesystem::exists(fsPath.data()))
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
                    std::filesystem::create_directories(fsPath.substr(0, fsPath.rfind('/')).data());
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

        return mount.vfsPath[mount.vfsPath.size() - 1] == '/' ||
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
            return mount.fsPath + '/' + std::string(path.data() + mount.vfsPath.size() + 1);
        }
    }

    void VFS::WriteText(const std::string& path, const std::string& text)
    {
        auto fsPath = ResolveFSPath(path, true);
        if (!fsPath.has_value())
        {
            if (!path.starts_with(".") && !path.starts_with("/") && path[1] != ':')
            {
                debug::Log::Error("Cannot open file {0}!", path);
                return;
            }

            fsPath = path;
        }

        std::ofstream myfile;
        myfile.open(fsPath.value().data());
        myfile << text;
        myfile.close();
    }

    bool VFS::Exists(const std::string& path)
    {
        return ResolveFSPath(path, false).has_value();
    }

    void VFS::ForEachFile(const std::string& dirPath, bool recursive, const std::function<void(const VFSFile&)>& func, bool parallel, bool sorted)
    {
        SPARK_ASSERT(!sorted || (!parallel && !recursive), "ForEachFile - sorted not supported when parallel or recursive are set.");

        auto fsPath = ResolveFSPath(dirPath, false);
        if (!fsPath.has_value())
        {
            return;
        }

        auto processFile = [this, func](const std::filesystem::directory_entry& file)
        {
            VFSFile vfsFile = {
                .fullPath = GetVFSPath(file.path().string()),
                .isDirectory = file.is_directory()
            };
            util::SplitPath(vfsFile.fullPath, vfsFile.dir, vfsFile.fileName, vfsFile.extension);
            func(vfsFile);
        };

        if (recursive)
        {
            if (parallel)
            {
                std::vector<std::filesystem::directory_entry> entries;
                for (const auto& e : std::filesystem::recursive_directory_iterator(fsPath.value().data()))
                {
                    entries.push_back(e);
                }

                threads::ParallelForEach(entries, processFile);
            }
            else
            {
                std::ranges::for_each(std::filesystem::recursive_directory_iterator(fsPath.value().data()), processFile);
            }
        }
        else
        {
            if (parallel)
            {
                std::vector<std::filesystem::directory_entry> entries;
                for (const auto& e : std::filesystem::directory_iterator(fsPath.value().data()))
                {
                    entries.push_back(e);
                }

                threads::ParallelForEach(entries, processFile);
            }
            else if (sorted)
            {
                std::vector<std::filesystem::directory_entry> sortedEntries;
                for (auto& entry : std::filesystem::directory_iterator(fsPath.value().data()))
                    sortedEntries.push_back(entry);

                std::ranges::sort(sortedEntries, [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b)
                {
                    return a.path() < b.path();
                });
                for (auto& entry : sortedEntries)
                    processFile(entry);
            }
            else
            {
                std::ranges::for_each(std::filesystem::directory_iterator(fsPath.value().data()), processFile);
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
                std::string dir, fileName;
                string::util::Split(temp, dir, fileName, '/', true);
#if SPARK_PLATFORM_WINDOWS
                FixWindowsPath(temp);
#endif
                return mount.vfsPath + std::string(temp.data() + mount.fsPath.size());
            }
        }

        return "";
    }

#if SPARK_PLATFORM_WINDOWS
    void VFS::FixWindowsPath(std::string& path)
    {
        std::replace(path.begin(), path.end(), '\\', '/');
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

    std::time_t VFS::GetLastModified(const std::string& path)
    {
        auto result = ResolveFSPath(path, false);
        if (result.has_value())
        {
            std::chrono::time_point<std::chrono::file_clock> timePoint = std::filesystem::last_write_time(result.value().data());
            return to_time_t(timePoint);
        }

        return 0;
    }
}
