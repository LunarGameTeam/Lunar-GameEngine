#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
namespace luna::graphics
{

void PBRPreparePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void OpaquePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void DirectionalLightShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void PointShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void PostProcessPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void OverlayPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

}
