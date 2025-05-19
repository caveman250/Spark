#pragma once
#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::asset::shader::ast
{
    SPARK_ENUM_BEGIN(OperatorType, int)
        Multiply,
        MultiplyEquals,
        Divide,
        DivideEquals,
        Add,
        AddEquals,
        Subtract,
        SubtractEquals,
        Equals,
        Compare,
        Less,
        LessEquals,
        Greater,
        GreaterEquals,
    SPARK_ENUM_END()

    class OperatorUtil
    {
    public:
        static uint8_t GetOperatorPriority(OperatorType type);
        static OperatorType StringToOperatorType(const std::string& token);
        static std::string OperatorTypeToGlsl(OperatorType type);
        static bool IsOperator(const std::string& str);
        static const std::vector<std::string>& GetOperatorStrings();
    private:
        static std::map<OperatorType, uint8_t> s_OperatorPriorities;
        static std::map<std::string, OperatorType> s_ShaderLangToType;
        static std::map<OperatorType, std::string> s_TypeToGlsl;
    };
}
