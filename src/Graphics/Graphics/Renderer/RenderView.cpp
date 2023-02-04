#include "Graphics/Renderer/RenderView.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderContext.h"

#include "Graphics/Renderer/ScenePipeline.h"

namespace luna::render
{

RenderView::RenderView() :
	mRT(nullptr)
{	
	auto device = sRenderModule->GetRenderContext();
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
	uint32_t shadowmapIdx = 0;
	for (PointLight* it : mOwnerScene->mPointLights)
	{
		it->Update(this);
		if (it->mCastShadow)
		{
			mOwnerScene->mSceneParamsBuffer->Set("cLightViewMatrix", it->mViewMatrix[0], shadowmapIdx);
			mOwnerScene->mSceneParamsBuffer->Set("cLightProjMatrix", it->mProjMatrix);
			shadowmapIdx++;
		}
	}
	mOwnerScene->mSceneParamsBuffer->Commit();
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