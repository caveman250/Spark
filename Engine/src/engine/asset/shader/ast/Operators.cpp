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
DEFINE_SPARK_ENUM_END()

namespace se::asset::shader::ast
{
    std::map<OperatorType::Type, uint8_t> OperatorUtil::s_OperatorPriorities =
    {
        {OperatorType::Multiply, 0u},
        {OperatorType::MultiplyEquals, 0u},
        {OperatorType::Divide, 0u},
        {OperatorType::DivideEquals, 0u},
        {OperatorType::Add, 1u},
        {OperatorType::AddEquals, 1u},
        {OperatorType::Subtract, 1u},
        {OperatorType::SubtractEquals, 1u},
        {OperatorType::Equals, 1u},
        {OperatorType::Compare, 2u},
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
}
