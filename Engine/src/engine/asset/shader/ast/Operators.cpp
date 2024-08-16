#include "Operators.h"

DEFINE_SPARK_ENUM_BEGIN(se::asset::shader::ast::OperatorType)
    DEFINE_ENUM_VALUE(OperatorType, Multiply)
    DEFINE_ENUM_VALUE(OperatorType, MultiplyEquals)
    DEFINE_ENUM_VALUE(OperatorType, Divide)
    DEFINE_ENUM_VALUE(OperatorType, DivideEquals)
    DEFINE_ENUM_VALUE(OperatorType, Add)
    DEFINE_ENUM_VALUE(OperatorType, AddEquals)
    DEFINE_ENUM_VALUE(OperatorType, Subtract)
    DEFINE_ENUM_VALUE(OperatorType, SubtractEquals)
    DEFINE_ENUM_VALUE(OperatorType, Equals)
    DEFINE_ENUM_VALUE(OperatorType, Compare)
    DEFINE_ENUM_VALUE(OperatorType, Less)
    DEFINE_ENUM_VALUE(OperatorType, LessEquals)
    DEFINE_ENUM_VALUE(OperatorType, Greater)
    DEFINE_ENUM_VALUE(OperatorType, GreaterEquals)
DEFINE_SPARK_ENUM_END()

namespace se::asset::shader::ast
{
    std::map<OperatorType::Type, uint8_t> OperatorUtil::s_OperatorPriorities =
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

    std::map<std::string, OperatorType::Type> OperatorUtil::s_ShaderLangToType =
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
    };

    std::map<OperatorType::Type, std::string> OperatorUtil::s_TypeToGlsl =
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
    };

    uint8_t OperatorUtil::GetOperatorPriority(OperatorType::Type type)
    {
        return s_OperatorPriorities[type];
    }

    OperatorType::Type OperatorUtil::StringToOperatorType(const std::string& token)
    {
        return s_ShaderLangToType[token];
    }

    std::string OperatorUtil::OperatorTypeToGlsl(OperatorType::Type type)
    {
        return s_TypeToGlsl[type];
    }

    bool OperatorUtil::IsOperator(const std::string& str)
    {
        return s_ShaderLangToType.contains(str);
    }

    const std::vector<std::string>& OperatorUtil::GetOperatorStrings()
    {
        static std::vector<std::string> ret = {"*", "/", "+", "-", "*=", "/=", "+=", "-=", "=", "<", ">", "<=", ">="};
        return ret;
    }
}
