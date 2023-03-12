#include"Graphics/Renderer/ScenePipeline.h"
#include "Graphics/Renderer/RenderScene.h"
namespace luna::render
{

	void RoPassProcessorManager::Init()
	{
		RegistorPassProcessor(MeshRenderPass::DirectLightShadowDepthPass, render::ShadowDepthPassProcessorCreateFunction);
		RegistorPassProcessor(MeshRenderPass::PointLightShadowDepthPass, render::PointShadowPassProcessorCreateFunction);
		RegistorPassProcessor(MeshRenderPass::LightingPass, render::OpaquePassProcessorCreateFunction);
	}
}