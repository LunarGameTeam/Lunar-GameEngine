#include "render_scene.h"
#include "render/render_module.h"

namespace luna::render
{

struct PerObjectBuffer
{
	LMatrix4f mWorldMatrix;
};

struct ObjectBuffer
{
	PerObjectBuffer mObjectBuffer[128];
};

struct SceneBuffer
{
	LVector3f mLightDirection;
	LVector4f mLightDiffuseColor;
};

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

void RenderScene::CommitSceneBuffer()
{
	if (!mBufferDirty)
		return;

	mBufferDirty = true;

	ObjectBuffer objectBuffer;
	SceneBuffer sceneBuffer;
	
	if (mDirLight)
	{
		sceneBuffer.mLightDirection = mDirLight->mDirection;
		sceneBuffer.mLightDiffuseColor = mDirLight->mColor;
	}
	else 
	{
		sceneBuffer.mLightDirection = LVector3f(0, 0, 1);
		sceneBuffer.mLightDiffuseColor = LVector4f(1, 1, 1, 1);
	}
		

	sceneBuffer.mLightDirection.normalize();
	for (auto& ro : mRenderObjects)
	{
		uint32_t idx = ro->mID % 128;
		PerObjectBuffer& perObject = objectBuffer.mObjectBuffer[idx];
		perObject.mWorldMatrix =*(ro->mWorldMat);
	}
	sRenderModule->mRenderDevice->UpdateConstantBuffer(mROBuffer, &objectBuffer, sizeof(ObjectBuffer));
	sRenderModule->mRenderDevice->UpdateConstantBuffer(mSceneBuffer, &sceneBuffer, sizeof(SceneBuffer));
}

RenderLight* RenderScene::CreateMainDirLight()
{
	mDirLight = new RenderLight();
	return mDirLight;
}

RenderView* RenderScene::CreateRenderView()
{
	RenderView* newView = new RenderView(mViews.size());
	mViews.push_back(newView);
	return newView;
}

void RenderScene::Render(FrameGraphBuilder* FG)
{
	if (!mInit)
	{

		RHIBufferDesc desc;
		desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
		desc.mSize = sizeof(ObjectBuffer);
		mROBuffer = sRenderModule->GetRenderDevice()->CreateBuffer(desc);


		desc.mSize = sizeof(SceneBuffer);
		mSceneBuffer = sRenderModule->GetRenderDevice()->CreateBuffer(desc);

		ViewDesc viewDesc;
		viewDesc.mViewType = RHIViewType::kConstantBuffer;
		viewDesc.mViewDimension = RHIViewDimension::BufferView;
		mROBufferView = sRenderModule->GetRenderDevice()->CreateView(viewDesc);
		mSceneBufferView = sRenderModule->GetRenderDevice()->CreateView(viewDesc);

		mROBufferView->BindResource(mROBuffer);
		mSceneBufferView->BindResource(mSceneBuffer);
		mInit = true;
	}

	CommitSceneBuffer();

	for (auto& renderView : mViews)
	{
		renderView->PrepareViewBuffer();
	}

	for (auto& renderView : mViews)
	{
		renderView->ScenePipeline(this, FG);		
	}
}

}
