module;

#include <string>

export module Spark.Asset.Shader:NameGenerator;

namespace se::asset::shader::ast
{
    export class NameGenerator
    {
    public:
        static std::string GetName();
    private:
        static uint64_t s_ID;
    };
}
