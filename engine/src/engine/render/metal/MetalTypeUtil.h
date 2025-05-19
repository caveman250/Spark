#pragma once

#import <Metal/Metal.h>
#include "engine/render/RenderState.h"

MTLCompareFunction DepthCompareToMtl(se::render::DepthCompare::Type depthCompare);
MTLBlendFactor BlendModeToMtl(se::render::BlendMode::Type blendMode);