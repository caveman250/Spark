#include "Operators.h"

namespace se::shader::ast
{
    std::map<OperatorType, uint8_t> OperatorUtil::s_OperatorPriorities =
    {
        {OperatorType::Multiply, 0u},
        {OperatorType::MultiplyEquals, 0u},
        {OperatorType::Divide, 0u},
        {OperatorType::DivideEquals, 0u},
        {OperatorType::Add, 1u},
        {OperatorType::AddEquals, 1u},
        {OperatorType::Subtract, 1u},
        {OperatorType::SubtractEquals, 1u},
        {OperatorType::Compare, 2u},
    };

    std::map<std::string, OperatorType> OperatorUtil::s_ShaderLangToType =
    {
        {"*", OperatorType::Multiply},
        {"*=", OperatorType::MultiplyEquals},
        {"/", OperatorType::Divide},
        {"/", OperatorType::DivideEquals},
        {"+", OperatorType::Add},
        {"+", OperatorType::AddEquals},
        {"-", OperatorType::Subtract},
        {"-", OperatorType::SubtractEquals},
        {"==", OperatorType::Compare},
    };

    std::map<OperatorType, std::string> OperatorUtil::s_TypeToGlsl =
    {
        {OperatorType::Multiply, "*"},
        {OperatorType::MultiplyEquals, "*="},
        {OperatorType::Divide, "/"},
        {OperatorType::DivideEquals, "/"},
        {OperatorType::Add, "+"},
        {OperatorType::AddEquals, "+"},
        {OperatorType::Subtract, "-"},
        {OperatorType::SubtractEquals, "-"},
        {OperatorType::Compare, "=="},
    };

    uint8_t OperatorUtil::GetOperatorPriority(OperatorType type)
    {
        return s_OperatorPriorities[type];
    }

    OperatorType OperatorUtil::StringToOperatorType(const std::string& token)
    {
        return s_ShaderLangToType[token];
    }

    std::string OperatorUtil::OperatorTypeToGlsl(OperatorType type)
    {
        return s_TypeToGlsl[type];
    }

    bool OperatorUtil::IsOperator(const std::string& str)
    {
        return s_ShaderLangToType.contains(str);
    }
}
