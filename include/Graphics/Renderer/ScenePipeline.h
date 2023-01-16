#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"


namespace luna::render
{

void LightingPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void ShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void OverlayPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

}
