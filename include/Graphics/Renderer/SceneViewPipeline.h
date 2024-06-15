#pragma once
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/FrameGraph/FrameGraph.h"
#include "Graphics/Renderer/RenderView.h"
namespace luna::graphics
{

LSharedPtr<RenderObjectDrawBatch> RunFilterROJob(RenderView* curView);
void SceneViewPipeline(FrameGraphBuilder* frameGraph, RenderScene* scene, RenderView* curView);
void ShadowCastViewPipeline(FrameGraphBuilder* frameGraph, RenderScene* scene, RenderView* curView);

}