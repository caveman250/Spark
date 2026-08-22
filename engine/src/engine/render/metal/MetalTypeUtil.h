#pragma once

#import <Metal/Metal.h>

MTLCompareFunction DepthCompareToMtl(int depthCompare);
MTLBlendFactor BlendModeToMtl(int blendMode);