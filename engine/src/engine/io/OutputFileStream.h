#pragma once

namespace se::io
{
    class OutputFileStream
    {
    public:
        OutputFileStream(const std::string& path, bool binary);
        ~OutputFileStream();

        OutputFileStream& operator<<(const std::string& str);
        void WriteBinary(const char* data, uint32_t size);

        void Close();
    private:
        std::ofstream m_Stream = {};
    };
}
