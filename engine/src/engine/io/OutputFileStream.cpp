#include "OutputFileStream.h"

#include "VFS.h"

namespace se::io
{
    OutputFileStream::OutputFileStream(const std::string& path, bool binary)
    {
        auto fsPath = VFS::Get().ResolveFSPath(path, true);
        if (fsPath.has_value())
        {
            m_Stream.open(fsPath.value().Data(), binary ? std::ios::binary | std::ios::out : std::ios::out);
        }
        else
        {
            debug::Log::Error("Unable to open file at {}", path);
        }
    }

    OutputFileStream::~OutputFileStream()
    {
        if (m_Stream.is_open())
        {
            Close();
        }
    }

    OutputFileStream& OutputFileStream::operator<<(const std::string& str)
    {
        if (SPARK_VERIFY(m_Stream.is_open()))
        {
            m_Stream << str;
        }

        return *this;
    }

    void OutputFileStream::WriteBinary(const char* data, uint32_t size)
    {
        if (SPARK_VERIFY(m_Stream.is_open()))
        {
            m_Stream.write(data, size);
        }
    }

    void OutputFileStream::Close()
    {
        if (SPARK_VERIFY(m_Stream.is_open()))
        {
            m_Stream.close();
        }
    }
}
