#pragma once
#include "spark.h"

namespace se::shader::ast
{
    enum class OperatorType
    {
        Multiply,
        MultiplyEquals,
        Divide,
        DivideEquals,
        Add,
        AddEquals,
        Subtract,
        SubtractEquals,
        Compare,
    }; // im sure ive missed some...
    class OperatorUtil
    {
    public:
        static uint8_t GetOperatorPriority(OperatorType type);
        static OperatorType StringToOperatorType(const std::string& token);
        static std::string OperatorTypeToGlsl(OperatorType type); // todo arena
        static bool IsOperator(const std::string& str);
    private:
        static std::map<OperatorType, uint8_t> s_OperatorPriorities;
        static std::map<std::string, OperatorType> s_ShaderLangToType;
        static std::map<OperatorType, std::string> s_TypeToGlsl;
    };
}
