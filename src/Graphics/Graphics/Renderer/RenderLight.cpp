#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Asset/MeshAsset.h"

namespace luna::render
{


void DirectionLight::Update(RenderView* view)
{
	if(!mParamBuffer)
		mParamBuffer = new ShaderParamsBuffer(sRenderModule->GetRenderContext()->mDefaultShader->GetConstantBufferDesc(LString("ViewBuffer").Hash()));
	LTransform transform = LTransform::Identity();
	auto rota = LQuaternion::FromTwoVectors(LVector3f(0, 0, 1), mDirection);
	transform.rotate(rota);
	mViewMatrix = transform.matrix().inverse();
	LMatrix4f proj;
	LMath::GenOrthoLHMatrix(proj, 30, 30, 0.01, 50);
	mParamBuffer->Set("cViewMatrix", mViewMatrix);
	mParamBuffer->Set("cProjectionMatrix", proj);
 	view->mOwnerScene->mSceneParamsBuffer->Set("cDirectionLightViewMatrix", mViewMatrix, 0);
	view->mOwnerScene->mSceneParamsBuffer->Set("cDirectionLightProjMatrix", proj, 0);
	mParamBuffer->Commit();
}

void PointLight::Update(RenderView* view)
{
	if (mCastShadow)
	{
		if (mParamBuffer.size() == 0)
		{
			const auto& desc = sRenderModule->GetRenderContext()->mDefaultShader->GetConstantBufferDesc(LString("ViewBuffer").Hash());
			mParamBuffer.push_back(new ShaderParamsBuffer(desc));
			mParamBuffer.push_back(new ShaderParamsBuffer(desc));
			mParamBuffer.push_back(new ShaderParamsBuffer(desc));
			mParamBuffer.push_back(new ShaderParamsBuffer(desc));
			mParamBuffer.push_back(new ShaderParamsBuffer(desc));
			mParamBuffer.push_back(new ShaderParamsBuffer(desc));
			mViewMatrix.resize(6);
		}
		LQuaternion rotation[] =
		{
			LMath::FromEuler(LVector3f(0,0,0)),
			LMath::FromEuler(LVector3f(0,90,0)),
			LMath::FromEuler(LVector3f(0,180,0)),
			LMath::FromEuler(LVector3f(0,270,0)),
			LMath::FromEuler(LVector3f(90,0,0)),
			LMath::FromEuler(LVector3f(-90,0,0)),
		};

		LMath::GenPerspectiveFovLHMatrix(mProjMatrix, mFov, mAspect, mNear, mFar);

		for (uint32_t idx = 0; idx < 6; idx++)
		{
			LTransform transform = LTransform::Identity();
			transform.translate(mPosition);			
			transform.rotate(rotation[idx]);
			mViewMatrix[idx] = transform.matrix().inverse();
			mParamBuffer[idx]->Set("cViewMatrix", mViewMatrix[idx]);
			mParamBuffer[idx]->Set("cProjectionMatrix", mProjMatrix);
			view->mOwnerScene->mSceneParamsBuffer->Set("cLightViewMatrix", mViewMatrix[idx], idx);
			view->mOwnerScene->mSceneParamsBuffer->Set("cLightProjMatrix", mProjMatrix, idx);
		}
		for (uint32_t idx = 0; idx < 6; idx++)
		{
			mParamBuffer[idx]->Commit();
		}
	}
	
}

}