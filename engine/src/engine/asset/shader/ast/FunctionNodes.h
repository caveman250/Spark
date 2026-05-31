#pragma once

#include "AbstractFunctionNode.h"

namespace se::asset::shader::ast
{
    class AbsNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        AbsNode() : AbstractFunctionNode("AbsNode", "abs", "abs") {}
    };

    class ClampNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        ClampNode() : AbstractFunctionNode("ClampNode", "clamp", "clamp") {}
    };

    class DFDXNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        DFDXNode() : AbstractFunctionNode("DFDXNode", "dFdx", "dfdx") {}
    };

    class DFDYNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        DFDYNode() : AbstractFunctionNode("DFDYNode", "dFdy", "dfdy") {}
    };

    class DotNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        DotNode() : AbstractFunctionNode("DotNode", "dot", "dot") {}
    };

    class FWidthNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        FWidthNode() : AbstractFunctionNode("FWidthNode", "fwidth", "fwidth") {}
    };

    class InverseNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        InverseNode() : AbstractFunctionNode("InverseNode", "inverse", "inverse") {}
    };

    class LengthNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        LengthNode() : AbstractFunctionNode("LengthNode", "length", "length") {}
    };

    class MaxNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        MaxNode() : AbstractFunctionNode("MaxNode", "max", "max") {}
    };

    class MinNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        MinNode() : AbstractFunctionNode("MinNode", "min", "min") {}
    };

    class ModNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        ModNode() : AbstractFunctionNode("ModNode", "mod", "") {}

        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override
        {
            auto alloc = outShader.get_allocator();
            SPARK_ASSERT(m_Children.size() == 2);
            const auto* child1 = m_Children[0].get();
            const auto* child2 = m_Children[1].get();

            outShader.append("(");
            child1->ToMtl(context, outShader);
            outShader.append(") - (");
            child2->ToMtl(context, outShader);
            outShader.append(")");

            outShader.append(" * floor((");
            child1->ToMtl(context, outShader);
            outShader.append(") / (");
            child2->ToMtl(context, outShader);
            outShader.append("))");
        }
    };

    class NormalizeNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        NormalizeNode() : AbstractFunctionNode("NormalizeNode", "normalize", "normalize") {}
    };

    class PowNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        PowNode() : AbstractFunctionNode("PowNode", "pow", "pow") {}
    };

    class ReflectNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        ReflectNode() : AbstractFunctionNode("ReflectNode", "reflect", "reflect") {}
    };

    class SmoothstepNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        SmoothstepNode() : AbstractFunctionNode("SmoothstepNode", "smoothstep", "smoothstep") {}
    };

    class SqrtNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        SqrtNode() : AbstractFunctionNode("SqrtNode", "sqrt", "sqrt") {}
    };

    class Atan2Node : public AbstractFunctionNode
    {
        SPARK_CLASS()
        Atan2Node() : AbstractFunctionNode("Atan2Node", "atan", "atan2") {}
    };

    class FractNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        FractNode() : AbstractFunctionNode("FractNode", "fract", "fract") {}
    };

    class StepNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        StepNode() : AbstractFunctionNode("StepNode", "step", "step") {}
    };

    class LerpNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        LerpNode() : AbstractFunctionNode("LerpNode", "mix", "mix") {}
    };

    class TransposeNode : public AbstractFunctionNode
    {
        SPARK_CLASS()
        TransposeNode() : AbstractFunctionNode("TransposeNode", "transpose", "transpose") {}
    };
}