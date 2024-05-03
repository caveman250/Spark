#include "Operators.h"

namespace se::shader::ast
{
    std::map<OperatorType, uint8_t> OperatorUtil::s_OperatorPriorities =
    {
        {OperatorType::Multiply, 0ui8},
        {OperatorType::MultiplyEquals, 0ui8},
        {OperatorType::Divide, 0ui8},
        {OperatorType::DivideEquals, 0ui8},
        {OperatorType::Add, 1ui8},
        {OperatorType::AddEquals, 1ui8},
        {OperatorType::Subtract, 1ui8},
        {OperatorType::SubtractEquals, 1ui8},
        {OperatorType::Compare, 2ui8},
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
