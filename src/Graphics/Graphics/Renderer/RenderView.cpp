#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/RenderModule.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/Renderer/ScenePipeline.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/RenderDevice.h"

namespace luna::render
{

RenderView::RenderView(uint64_t view_id) :
	mRT(nullptr),
	mViewID(view_id)
{	
	auto device = sRenderModule->GetRenderDevice();
	mViewBuffer = new ShaderParamsBuffer(device->mDefaultShader->GetConstantBufferDesc(LString("ViewBuffer").Hash()));
}

void RenderView::PrepareView()
{
	mViewBuffer->Set("cViewMatrix", mViewMatrix);
	mViewBuffer->Set("cProjectionMatrix", mProjMatrix);
	LVector2f cNearFar(mNear, mFar);
	mViewBuffer->Set("cNearFar", cNearFar);
	if (mOwnerScene->mMainDirLight)
	{
		mOwnerScene->mMainDirLight->Update(this);
	}
	mViewBuffer->Commit();

}

void RenderView::Culling(RenderScene* scene)
{
	mViewVisibleROs.clear();
	mViewVisibleROs = scene->GetRenderObjects();
}

void RenderView::Render(RenderScene* scene, FrameGraphBuilder* FG)
{
	Culling(scene);

	switch (mViewType)
	{
	case RenderViewType::SceneView:
	{
		ShadowPass(FG, this, scene);
		OpaquePass(FG, this, scene);
		OverlayPass(FG, this, scene);
		break;
	}	
	case RenderViewType::ShadowMapView:
	{
		break;
	}
	default:
		break;
	}


}

}