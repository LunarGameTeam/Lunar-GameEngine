#include "render_pass.h"

#include "core/math/math.h"

#include "render/renderer/render_scene.h"
#include "render/renderer/render_target.h"
#include "render/render_module.h"

#include <core/asset/asset_module.h>


namespace luna::render
{


void RenderPass::Init()
{
	m_is_init = true;
	InitImpl();
}

void RenderPass::InitImpl()
{

}

void RenderPass::OnRenderPassPrepare()
{
	
}

void RenderPass::BuildRenderPass(FrameGraphBuilder* fg, RenderView* view, RenderScene* renderScene)
{
	mView = view;
	mRenderScene = renderScene;		
}

void RenderPass::SetupPassBuffer()
{


}

}
