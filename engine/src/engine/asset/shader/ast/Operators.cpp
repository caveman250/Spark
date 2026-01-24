#include "Operators.h"

namespace se::asset::shader::ast
{
    std::map<OperatorType, uint8_t> OperatorUtil::s_OperatorPriorities =
    {
        { OperatorType::Multiply, static_cast<uint8_t>(0u) },
        { OperatorType::MultiplyEquals, static_cast<uint8_t>(0u) },
        { OperatorType::Divide, static_cast<uint8_t>(0u) },
        { OperatorType::DivideEquals, static_cast<uint8_t>(0u) },
        { OperatorType::Add, static_cast<uint8_t>(1u) },
        { OperatorType::AddEquals, static_cast<uint8_t>(1u) },
        { OperatorType::Subtract, static_cast<uint8_t>(1u) },
        { OperatorType::SubtractEquals, static_cast<uint8_t>(1u) },
        { OperatorType::Equals, static_cast<uint8_t>(1u) },
        { OperatorType::Compare, static_cast<uint8_t>(2u) },
        { OperatorType::Less, static_cast<uint8_t>(2u) },
        { OperatorType::LessEquals, static_cast<uint8_t>(2u) },
        { OperatorType::Greater, static_cast<uint8_t>(2u) },
        { OperatorType::GreaterEquals, static_cast<uint8_t>(2u) },
    };

    std::map<std::string, OperatorType> OperatorUtil::s_ShaderLangToType =
    {
        {"*", OperatorType::Multiply},
        {"*=", OperatorType::MultiplyEquals},
        {"/", OperatorType::Divide},
        {"/=", OperatorType::DivideEquals},
        {"+", OperatorType::Add},
        {"+=", OperatorType::AddEquals},
        {"-", OperatorType::Subtract},
        {"-=", OperatorType::SubtractEquals},
        {"=", OperatorType::Equals },
        {"==", OperatorType::Compare},
        {"<", OperatorType::Less},
        {"<=", OperatorType::LessEquals},
        {">", OperatorType::Greater},
        {">=", OperatorType::GreaterEquals},
        {"||", OperatorType::Or},
        {"&&", OperatorType::And},
    };

    std::map<OperatorType, std::string> OperatorUtil::s_TypeToGlsl =
    {
        {OperatorType::Multiply, "*"},
        {OperatorType::MultiplyEquals, "*="},
        {OperatorType::Divide, "/"},
        {OperatorType::DivideEquals, "/="},
        {OperatorType::Add, "+"},
        {OperatorType::AddEquals, "+="},
        {OperatorType::Subtract, "-"},
        {OperatorType::SubtractEquals, "-="},
        {OperatorType::Equals, "="},
        {OperatorType::Compare, "=="},
        {OperatorType::Less, "<"},
        {OperatorType::LessEquals, "<="},
        {OperatorType::Greater, ">"},
        {OperatorType::GreaterEquals, ">="},
        {OperatorType::Or, "||"},
        {OperatorType::And, "&&"},
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

    const std::vector<std::string>& OperatorUtil::GetOperatorStrings()
    {
        static std::vector<std::string> ret = {"*", "/", "+", "-", "*=", "/=", "+=", "-=", "=", "<", ">", "<=", ">=", "||", "&&"};
        return ret;
    }
}
