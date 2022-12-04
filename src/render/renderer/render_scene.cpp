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

RenderScene::RenderScene(int32_t scene_id) : mViews(this)
{
	mSceneID = scene_id;
}


RenderObject* RenderScene::CreateRenderObject()
{
	RenderObject* ro = new RenderObject();
	ro->mID = mRenderObjects.size();
	mRenderObjects.push_back(ro);
	return ro;
}

void RenderScene::UpdateRenderObject()
{

	ObjectBuffer objectBuffer;
	SceneBuffer sceneBuffer;
	sceneBuffer.mLightDirection = LVector3f(1,1,1);
	sceneBuffer.mLightDiffuseColor = LVector4f(1, 1, 1, 1);
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

Light* RenderScene::AddMainLight()
{
	mDirLight.mCastShadow = false;
// 	mDirLight.mColor = light_value->GetColor();
// 	mDirLight.mDirection = light_value->GetDirection();
// 	mDirLight.mIndensity = light_value->GetIntensity();
// 	mDirLight.mViewMatrix = light_value->GetViewMatrix();
// 	mDirLight.mProjMatrix = light_value->GetProjectionMatrix(0);
	mAllLight.push_back(mDirLight);
	return &mAllLight.back();
}

RenderView* RenderScene::CreateRenderView()
{
	RenderView* new_view = new RenderView(mViews.Size());
	mViews.PushBack(new_view);
	return new_view;
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

	UpdateRenderObject();

	for (auto& renderView : mViews)
	{
		renderView->PrepareViewBuffer();
	}

	for (auto& renderView : mViews)
	{
		
		switch (renderView->mViewType)
		{
		case RenderViewType::SceneView:
		{
			renderView->GenerateSceneViewPass(this, FG);			
		}
		break;
		case RenderViewType::CSMShadowView:
		{
			renderView->GenerateShadowViewPass(this, FG);			
		}
		break;
		default:
			break;
		}

	}
}

}
