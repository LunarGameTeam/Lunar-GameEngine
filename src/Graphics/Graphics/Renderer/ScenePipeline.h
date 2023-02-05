#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"


namespace luna::render
{

void OpaquePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void DirectionalLightShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void PointShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void OverlayPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

}
