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
	auto translate = LMath::GetMatrixTranslaton(view->GetViewMatrix());
	transform.translate(translate);
	auto rota = LQuaternion::FromTwoVectors(LVector3f(1, 0, 0), mDirection);
	transform.rotate(rota);
	LMatrix4f viewMat = transform.matrix().inverse();
	LMatrix4f proj;
	LMath::GenOrthoLHMatrix(proj, 30, 30, 0.01, 50);
	mParamBuffer->Set("cViewMatrix", viewMat);
	mParamBuffer->Set("cProjectionMatrix", proj);
	view->mOwnerScene->mSceneParamsBuffer->Set("cLightViewMatrix", viewMat, 0);
	view->mOwnerScene->mSceneParamsBuffer->Set("cLightProjMatrix", proj, 0);
	mParamBuffer->Commit();
}

void PointLight::Update(RenderView* view)
{
	if (mParamBuffer.size() == 0)
	{
		const auto& desc = sRenderModule->GetRenderContext()->mDefaultShader->GetConstantBufferDesc(LString("ViewBuffer").Hash());			
		mParamBuffer.push_back(new ShaderParamsBuffer(desc));
		mParamBuffer.push_back(new ShaderParamsBuffer(desc));
		mParamBuffer.push_back(new ShaderParamsBuffer(desc));
		mParamBuffer.push_back(new ShaderParamsBuffer(desc));
		mViewMatrix.resize(8);
	}
	LTransform transform = LTransform::Identity();
	transform.translate(mPosition);
	mViewMatrix[0] = transform.matrix().inverse();
	LMath::GenPerspectiveFovLHMatrix(mProjMatrix, mFov, mAspect, mNear, mFar);
	mParamBuffer[0]->Set("cViewMatrix", mViewMatrix[0]);
	mParamBuffer[0]->Set("cProjectionMatrix", mProjMatrix);
	mParamBuffer[0]->Commit();
}

}