#pragma once

namespace se::shader::ast
{
    class NameGenerator
    {
    public:
        static std::string GetName();
    private:
        static uint64_t s_ID;
    };
}
