#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/RenderModule.h"

#include "Graphics/Asset/MeshAsset.h"

#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"

#include "Graphics/RHI/RHIResource.h"
#include "Graphics/FrameGraph/FrameGraph.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/RHI/RHIShader.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderLight.h"

namespace luna::render
{


RenderScene::RenderScene()
{

}


RenderObject* RenderScene::CreateRenderObject()
{
	RenderObject* ro = new RenderObject();
	ro->mID = mRenderObjects.size();
	mRenderObjects.push_back(ro);
	return ro;
}


void RenderScene::PrepareScene()
{
	if (!mBufferDirty)
		return;

	if (mDebugMesh == nullptr)
	{
		mDebugMesh = new SubMesh();
		RHIVertexLayout& vertexlayout = mDebugMesh->GetVertexLayout();
		mDebugMesh->Update();
	}

	if (mDebugMeshLine == nullptr)
	{
		mDebugMeshLine = new SubMesh();
		RHIVertexLayout& vertexlayout = mDebugMeshLine->GetVertexLayout();
		mDebugMeshLine->Update();
	}


	if (mSceneParamsBuffer == nullptr)
		mSceneParamsBuffer = new ShaderParamsBuffer(sRenderModule->GetRenderDevice()->mDefaultShader->GetConstantBufferDesc(LString("SceneBuffer").Hash()));
	if (mROIDInstancingBuffer == nullptr)
		mROIDInstancingBuffer = new ShaderParamsBuffer(RHIBufferUsage::VertexBufferBit, sizeof(uint32_t) * 4 * 128);

	if (mMainDirLight && !mMainDirLight->mInit)
		mMainDirLight->Init();

	if (mMainDirLight)
	{
		mSceneParamsBuffer->Set("cDirectionLightColor", mMainDirLight->mColor);
		mSceneParamsBuffer->Set("cLightDirection", mMainDirLight->mDirection);
	}
	else
	{
		mSceneParamsBuffer->Set("cDirectionLightColor", LVector4f(0,0,0,0));
	}
	
	
	mSceneParamsBuffer->Set("cPointLightsCount", mPointLights.size());
	for (int i = 0; i < mPointLights.size(); i++)
	{
		PointLight* light = mPointLights[i];
		mSceneParamsBuffer->Set("cPointLights", light->mPosition, i, 0);
		mDebugMeshLine->AddCubeWired(light->mPosition, LVector3f(1, 1, 1), light->mColor);
		mSceneParamsBuffer->Set("cPointLights", light->mColor, i, 16);
		mSceneParamsBuffer->Set("cPointLights", light->mIntensity, i, 32);
	}
	for (auto& ro : mRenderObjects)
	{		
		uint32_t idx = ro->mID;
		mSceneParamsBuffer->Set("cRoWorldMatrix", * ro->mWorldMat, idx);
		mROIDInstancingBuffer->Set(idx * sizeof(uint32_t) * 4, idx);
	}
	

	mSceneParamsBuffer->Commit();
	mROIDInstancingBuffer->Commit();
	
}

DirectionLight* RenderScene::CreateMainDirLight()
{
	mMainDirLight = new DirectionLight();
	return mMainDirLight;
}

PointLight* RenderScene::CreatePointLight()
{
	auto light = new PointLight();
	mPointLights.push_back(light);
	return light;
}

RenderView* RenderScene::CreateRenderView()
{
	RenderView* newView = new RenderView(mViews.size());
	newView->mOwnerScene = this;
	mViews.push_back(newView);
	return newView;
}

void RenderScene::Render(FrameGraphBuilder* FG)
{
	if(mDebugMesh)
		mDebugMesh->ClearVertexData();
	if(mDebugMeshLine)
		mDebugMeshLine->ClearVertexData();
	PrepareScene();
	
	mDebugMesh->Update();
	mDebugMeshLine->Update();
	for (RenderView* renderView : mViews)
	{
		renderView->PrepareView();
		renderView->Render(this, FG);		
	}
}

void RenderScene::DestroyRenderView(RenderView* renderView)
{
	for (auto it = mViews.begin(); it != mViews.end(); ++it)
	{
		if (renderView == *it)
		{			
			mViews.erase(it);			
			delete renderView;
			return;
		}
	}
}

void RenderScene::DestroyRenderObject(RenderObject* ro)
{
	for (auto it = mRenderObjects.begin(); it != mRenderObjects.end(); ++it)
	{
		if (ro == *it)
		{
			mRenderObjects.erase(it);
			delete ro;
			break;
		}
	}
	mBufferDirty = true;
}

RenderScene::~RenderScene()
{
	for (RenderView* it : mViews)
	{
		delete it;
	}
	if (mROIDInstancingBuffer)
	{
		delete mROIDInstancingBuffer;
		mROIDInstancingBuffer = nullptr;
	}
	for (PointLight* it : mPointLights)
	{
		delete it;
	}
	if (mSceneParamsBuffer)
	{
		delete mSceneParamsBuffer;
		mSceneParamsBuffer = nullptr;
	}
	mViews.clear();
	for (RenderObject* it : mRenderObjects)
	{
		delete it;
	}
	mRenderObjects.clear();
}

void RenderScene::DestroyMainDirLight(DirectionLight* val)
{
	if (mMainDirLight == val)
	{
		delete mMainDirLight;
		mMainDirLight = nullptr;
	}
}

}
