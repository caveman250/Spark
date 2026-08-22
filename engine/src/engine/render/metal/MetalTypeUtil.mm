#import "MetalTypeUtil.h"
#include "spark.h"

MTLCompareFunction DepthCompareToMtl(int depthCompare)
{
    switch (depthCompare)
    {
        case 0:
            return MTLCompareFunctionLess;
        case 1:
            return MTLCompareFunctionLessEqual;
        case 2:
            return MTLCompareFunctionEqual;
        case 3:
            return MTLCompareFunctionGreater;
        case 4:
            return MTLCompareFunctionGreaterEqual;
        case 5:
            return MTLCompareFunctionAlways;
        default:
            SPARK_ASSERT(false, "MetalTypeUtil::DepthCompareToMtl - Unhandled DepthCompare");
            return MTLCompareFunctionAlways;
    }
}

MTLBlendFactor BlendModeToMtl(int blendMode)
{
    switch (blendMode)
    {
        case 0:
            return MTLBlendFactorZero;
        case 1:
            return MTLBlendFactorOne;
        case 2:
            return MTLBlendFactorSourceColor;
        case 3:
            return MTLBlendFactorOneMinusSourceColor;
        case 4:
            return MTLBlendFactorDestinationColor;
        case 5:
            return MTLBlendFactorOneMinusDestinationColor;
        case 6:
            return MTLBlendFactorSourceAlpha;
        case 7:
            return MTLBlendFactorOneMinusSourceAlpha;
        case 8:
            return MTLBlendFactorDestinationAlpha;
        case 9:
            return MTLBlendFactorOneMinusDestinationAlpha;
        case 10:
            return MTLBlendFactorZero;
        default:
            SPARK_ASSERT(false, "MetalTypeUtil::BlendModeToMtl - Unhandled BlendMode");
            return MTLBlendFactorZero;
    }
}