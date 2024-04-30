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
                    return GetFSPath(vfsPath, mount);
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
}
