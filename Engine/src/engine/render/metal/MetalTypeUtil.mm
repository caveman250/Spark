#import "MetalTypeUtil.h"

MTLCompareFunction DepthCompareToMtl(se::render::DepthCompare::Type depthCompare)
{
    switch (depthCompare)
    {
        case se::render::DepthCompare::Less:
            return MTLCompareFunctionLess;
        case se::render::DepthCompare::LessEqual:
            return MTLCompareFunctionLessEqual;
        case se::render::DepthCompare::Equal:
            return MTLCompareFunctionEqual;
        case se::render::DepthCompare::Greater:
            return MTLCompareFunctionGreater;
        case se::render::DepthCompare::GreaterEqual:
            return MTLCompareFunctionGreaterEqual;
        case se::render::DepthCompare::None:
            return MTLCompareFunctionAlways;
        default:
            SPARK_ASSERT(false, "MetalTypeUtil::DepthCompareToMtl - Unhandled DepthCompare");
            return MTLCompareFunctionAlways;
    }
}

MTLBlendFactor BlendModeToMtl(se::render::BlendMode::Type blendMode)
{
    switch (blendMode)
    {
        case se::render::BlendMode::Zero:
            return MTLBlendFactorZero;
        case se::render::BlendMode::One:
            return MTLBlendFactorOne;
        case se::render::BlendMode::SrcColor:
            return MTLBlendFactorSourceColor;
        case se::render::BlendMode::OneMinusSrcColor:
            return MTLBlendFactorOneMinusSourceColor;
        case se::render::BlendMode::DstColor:
            return MTLBlendFactorDestinationColor;
        case se::render::BlendMode::OneMinusDstColor:
            return MTLBlendFactorOneMinusDestinationColor;
        case se::render::BlendMode::SrcAlpha:
            return MTLBlendFactorSourceAlpha;
        case se::render::BlendMode::OneMinusSrcAlpha:
            return MTLBlendFactorOneMinusSourceAlpha;
        case se::render::BlendMode::DstAlpha:
            return MTLBlendFactorDestinationAlpha;
        case se::render::BlendMode::OneMinusDstAlpha:
            return MTLBlendFactorOneMinusDestinationAlpha;
        case se::render::BlendMode::None:
            return MTLBlendFactorZero;
        default:
            SPARK_ASSERT(false, "MetalTypeUtil::BlendModeToMtl - Unhandled BlendMode");
            return MTLBlendFactorZero;
    }
}