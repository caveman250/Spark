#pragma once
#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::asset::shader::ast
{
    DECLARE_SPARK_ENUM_BEGIN(OperatorType, int)
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
    DECLARE_SPARK_ENUM_END()

    class OperatorUtil
    {
    public:
        static uint8_t GetOperatorPriority(OperatorType::Type type);
        static OperatorType::Type StringToOperatorType(const std::string& token);
        static std::string OperatorTypeToGlsl(OperatorType::Type type);
        static bool IsOperator(const std::string& str);
        static const std::vector<std::string>& GetOperatorStrings();
    private:
        static std::map<OperatorType::Type, uint8_t> s_OperatorPriorities;
        static std::map<std::string, OperatorType::Type> s_ShaderLangToType;
        static std::map<OperatorType::Type, std::string> s_TypeToGlsl;
    };
}
