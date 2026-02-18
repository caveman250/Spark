#include "ForLoopNode.h"

#include "BinaryExpressionNode.h"
#include "ConstantNode.h"
#include "ShaderCompileContext.h"
#include "VariableDeclarationNode.h"
#include "VariableReferenceNode.h"

namespace se::asset::shader::ast
{
    ForLoopNode::ForLoopNode(const ForLoopNode& rhs)
        : ASTNode(rhs)
    {
        m_Declaration.reserve(rhs.m_Declaration.size());
        for (const auto& child : rhs.m_Declaration)
        {
            auto* objBase = static_cast<ObjectBase*>(child.get());
            if (SPARK_VERIFY(objBase))
            {
                m_Declaration.push_back(std::shared_ptr<ASTNode>(static_cast<ASTNode*>(objBase->GetReflectType()->heap_copy_constructor(child.get()))));
            }
        }

        if (rhs.m_Condition)
        {
            const auto* objBase = static_cast<ObjectBase *>(rhs.m_Condition.get());
            m_Condition = std::shared_ptr<ASTNode>(static_cast<ASTNode*>(objBase->GetReflectType()->heap_copy_constructor(rhs.m_Condition.get())));
        }

        if (rhs.m_Expression)
        {
            const auto* objBase = static_cast<ObjectBase *>(rhs.m_Expression.get());
            m_Expression = std::shared_ptr<ASTNode>(static_cast<ASTNode*>(objBase->GetReflectType()->heap_copy_constructor(rhs.m_Expression.get())));
        }
    }

    void ForLoopNode::ApplyNameRemapping(const std::map<std::string, std::string>& newNames)
    {
        std::ranges::for_each(m_Declaration, [newNames](const auto& item) { item->ApplyNameRemapping(newNames); });
        m_Condition->ApplyNameRemapping(newNames);
        m_Expression->ApplyNameRemapping(newNames);
        ASTNode::ApplyNameRemapping(newNames);
    }

    void ForLoopNode::ForEachChild(const std::function<void(ASTNode*)>& func)
    {
        std::ranges::for_each(m_Declaration, [func](const auto& item) { func(item.get()); });
        func(m_Condition.get());
        func(m_Expression.get());
        ASTNode::ForEachChild(func);
    }

    const std::shared_ptr<ASTNode>& ForLoopNode::AddChild(ASTNode* node)
    {
        if (!m_DeclarationEnded)
        {
            return m_Declaration.emplace_back(std::shared_ptr<ASTNode>(node));
        }
        else if (!m_Condition)
        {
            return m_Condition = std::shared_ptr<ASTNode>(node);
        }
        else if (!m_Expression)
        {
            return m_Expression = std::shared_ptr<ASTNode>(node);
        }
        else
        {
            SPARK_ASSERT(m_ExpressionEnded);
            return ASTNode::AddChild(node);
        }
    }

    std::string ForLoopNode::GetDebugString() const
    {
        return "ForLoopNode";
    }

    void ForLoopNode::ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        std::ranges::for_each(m_Declaration, [&context, &outShader](const auto& item) { item->ToGlsl(context, outShader); });
        outShader += ";\nfor(;";
        m_Condition->ToGlsl(context, outShader);
        outShader += ";";
        m_Expression->ToGlsl(context, outShader);
        outShader += ")\n{\n";

        for (const auto& child : m_Children)
        {
            child->ToGlsl(context, outShader);
        }

        outShader += "}\n";
    }


    ShaderValue EvaluateForLoopInitialIteratorValue(const std::vector<std::shared_ptr<ASTNode>>& expression)
    {
        std::string variableName = {};
        for (const auto& node : expression)
        {
            if (variableName.size() == 0)
            {
                if (auto* decNode = dynamic_cast<VariableDeclarationNode*>(node.get()))
                {
                    variableName = decNode->GetName();
                }
            }

            if (auto* binaryExpression = dynamic_cast<BinaryExpressionNode*>(node.get()))
            {
                SPARK_ASSERT(variableName.size() > 0);
                // we expect the lhs to be variableName
                [[maybe_unused]] auto* refNode = dynamic_cast<VariableReferenceNode*>(binaryExpression->m_Children[0].get());
                SPARK_ASSERT(refNode);
                SPARK_ASSERT(strcmp(refNode->GetName().c_str(), variableName.data()) == 0);

                // TODO support for more complex expressions (also parser should catch this)
                auto* constantNode = dynamic_cast<ConstantNodeBase*>(binaryExpression->m_Children[1].get());
                if (SPARK_VERIFY(constantNode))
                {
                    return constantNode->GetValue();
                }
                return -1;
            }
        }

        SPARK_ASSERT(false);
        return -1;
    }

    // VERY BASIC
    struct ForLoopExpression
    {
        OperatorType opType;
        std::string lhs;
        ShaderValue rhs;
    };

    ForLoopExpression EvaluateForLoopCondition(const std::shared_ptr<ASTNode>& expression)
    {
        std::string variableName = {};
        SPARK_ASSERT(expression->GetReflectType() == reflect::TypeResolver<BinaryExpressionNode>::get());
        auto* binaryExpression = static_cast<BinaryExpressionNode*>(expression.get());
        auto* refNode = dynamic_cast<VariableReferenceNode*>(binaryExpression->m_Children[0].get());
        if (!SPARK_VERIFY(refNode))
        {
            return { .opType = OperatorType::Add,  .lhs = "", .rhs = -1 };
        }

        variableName = refNode->GetName();
        int amount = 1;

        if (binaryExpression->m_Children.size() > 1)
        {
            auto* constantNode = dynamic_cast<ConstantNode<int>*>(binaryExpression->m_Children[1].get());
            if (!SPARK_VERIFY(constantNode))
            {
                return { .opType = OperatorType::Add, .lhs = "", .rhs = -1 };
            }

            auto constantVal = constantNode->GetValue();
            SPARK_ASSERT(std::holds_alternative<int>(constantVal));
            amount = std::get<int>(constantVal);
        }

        return { .opType = binaryExpression->GetOperatorType(), .lhs = variableName, .rhs = amount };
    }

    void ForLoopNode::ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        // TODO Much stricter parsing

        // metal does not support arrays being interpolated from vertex shaders to fragment shaders.
        // this is a common occurrence in glsl, and a pattern I would like to keep.
        // to make this compatible with metal shaders, we replace arrays with multiple variables eg:
        // float3 var[2] becomes float3 var0, float3 var1;
        // to account for this, we also need to unroll our for loops at compile time

        ShaderValue initialIteratorValue = EvaluateForLoopInitialIteratorValue(m_Declaration);
        ForLoopExpression condition = EvaluateForLoopCondition(m_Condition);
        ForLoopExpression iteratorExpression = EvaluateForLoopCondition(m_Expression);

        SPARK_ASSERT(std::holds_alternative<int>(initialIteratorValue));
        int initialVal = std::get<int>(initialIteratorValue);

        SPARK_ASSERT(std::holds_alternative<int>(condition.rhs));
        int comp = std::get<int>(condition.rhs);

        SPARK_ASSERT(std::holds_alternative<int>(iteratorExpression.rhs));
        SPARK_ASSERT(iteratorExpression.opType == OperatorType::AddEquals || iteratorExpression.opType == OperatorType::SubtractEquals);
        int sign = iteratorExpression.opType == OperatorType::AddEquals ? 1 : -1;
        int loopIt = std::get<int>(iteratorExpression.rhs) * sign;

        for (int i = initialVal; i < comp; i += loopIt)
        {
            context.tempRenames = std::map<std::string, std::string>{ { condition.lhs.data(), std::to_string(i) } };
            outShader += "{\n";
            for (const auto& child : m_Children)
            {
                child->ToMtl(context, outShader);
            }
            outShader += "}\n";
        }

        context.tempRenames.clear();
    }
}
