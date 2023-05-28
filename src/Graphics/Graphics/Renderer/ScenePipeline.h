#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
namespace luna::render
{

void PBRPreparePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

RoPassProcessorBase* OpaquePassProcessorCreateFunction(RenderScene* scene, MeshRenderPass passType);
void OpaquePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

RoPassProcessorBase* ShadowDepthPassProcessorCreateFunction(RenderScene* scene, MeshRenderPass passType);
void DirectionalLightShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

RoPassProcessorBase* PointShadowPassProcessorCreateFunction(RenderScene* scene, MeshRenderPass passType);
void PointShadowPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

void PostProcessPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
void OverlayPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);

}
