#include "Operators.h"

namespace se::shader::ast
{
    std::map<OperatorType, uint8_t> OperatorUtil::s_OperatorPriorities =
    {
        {OperatorType::Multiply, 0},
        {OperatorType::MultiplyEquals, 0},
        {OperatorType::Divide, 0},
        {OperatorType::DivideEquals, 0},
        {OperatorType::Add, 1},
        {OperatorType::AddEquals, 1},
        {OperatorType::Subtract, 1},
        {OperatorType::SubtractEquals, 1},
        {OperatorType::Compare, 2},
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
