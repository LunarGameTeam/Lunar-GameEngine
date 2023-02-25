#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"


namespace luna::render
{

void PBRPreparePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void OpaquePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void DirectionalLightShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void PointShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void OverlayPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void PostProcessPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

}
