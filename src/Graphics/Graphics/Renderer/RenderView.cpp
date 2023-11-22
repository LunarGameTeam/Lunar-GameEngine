#include "Graphics/Renderer/RenderView.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderContext.h"

#include "Graphics/Renderer/ScenePipeline.h"

namespace luna::graphics
{
PARAM_ID(ViewBuffer);

RenderView::RenderView() :
	mRT(nullptr)
{	
	auto device = sRenderModule->GetRenderContext();
	mViewBuffer = new ShaderCBuffer(device->GetDefaultShaderConstantBufferDesc(LString("ViewBuffer").Hash()));
}

void RenderView::SetMaterialViewParameter(MaterialInstance* matInstance)
{
	matInstance->SetShaderInput(ParamID_ViewBuffer, mViewBuffer->mView);
}

void RenderView::PrepareView()
{
	if (!mDirty)
	{
		return;
	}
	mViewBuffer->Set("cViewMatrix", mViewMatrix);
	mViewBuffer->Set("cProjectionMatrix", mProjMatrix);
	LVector2f cNearFar(mNear, mFar);
	mViewBuffer->Set("cNearFar", cNearFar);
	mViewBuffer->Set("cCamPos", LMath::GetMatrixTranslaton(mViewMatrix.inverse()));	
	mViewBuffer->Commit();
}

void RenderView::Culling(RenderScene* scene)
{
	scene->GetRenderObjects(mViewVisibleROs);
}

}