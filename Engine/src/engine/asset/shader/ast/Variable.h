#pragma once

#include "spark.h"
#include "engine/asset/shader/ast/Types.h"

namespace se::asset::shader::ast
{
    struct Variable : reflect::ObjectBase
    {
        SPARK_CLASS()

        Variable() = default;
        Variable(const Variable&) = default;
        Variable& operator=(const Variable& rhs) = default;
        Variable(AstType::Type _type, std::string& _arraySize)
            : type(_type)
            , arraySizeVariable(_arraySize)
        {

        }

        Variable(AstType::Type _type, int _arraySize)
            : type(_type)
            , arraySizeConstant(_arraySize)
        {
        }

        bool operator==(const Variable& rhs) const
        {
            return type == rhs.type &&
                    arraySizeVariable == rhs.arraySizeVariable &&
                    arraySizeConstant == rhs.arraySizeConstant;
        }

        SPARK_MEMBER(Serialized)
        AstType::Type type = {};

        SPARK_MEMBER(Serialized)
        std::string arraySizeVariable = {};

        SPARK_MEMBER(Serialized)
        int arraySizeConstant = 0;
    };
}
