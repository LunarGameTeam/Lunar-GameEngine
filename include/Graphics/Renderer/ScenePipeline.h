#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"


namespace luna::render
{

void BuildRenderPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

}
