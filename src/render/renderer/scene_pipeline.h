#pragma once

#include "render/render_config.h"
#include "render/forward_types.h"


namespace luna::render
{

void BuildRenderPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

}
