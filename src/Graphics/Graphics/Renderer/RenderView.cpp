#include "Graphics/Renderer/RenderView.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderContext.h"
#include "Graphics/Renderer/SceneViewPipeline.h"


namespace luna::graphics
{
PARAM_ID(ViewBuffer);
void RenderViewParameterData::SetMaterialParameter(MaterialInstanceBase* matInstance)
{
	matInstance->SetShaderInput(ParamID_ViewBuffer, mViewParamCbufferView);
}

void RenderViewParameterData::PerViewUpdate(RenderView* renderView)
{
	if ((!mIntrinsicsDirty) && (!mExtrinsicsDirty))
	{
		return;
	}
	if(mIntrinsicsDirty)
	{
		float curAspect = (float)mIntrinsicsParameter.mRtWidth / (float)mIntrinsicsParameter.mRtHeight;
		LMath::GenPerspectiveFovLHMatrix(mProjMatrix,mIntrinsicsParameter.mFovY, curAspect, mIntrinsicsParameter.mNear, mIntrinsicsParameter.mFar);
	}
	LVector2f cNearFar(mIntrinsicsParameter.mNear, mIntrinsicsParameter.mFar);
	mViewCbuffer->Set("cNearFar", cNearFar);
	mViewCbuffer->Set("cProjectionMatrix", mProjMatrix);
	mViewCbuffer->Set("cViewMatrix", mExtrinsicsParameter.mViewMatrix);
	mViewCbuffer->Set("cCamPos", mExtrinsicsParameter.mPosition);
	renderView->mOwnerScene->AddCbufferCopyCommand(mViewCbuffer.get(), mViewParamRes.get());
	mIntrinsicsDirty = false;
	mExtrinsicsDirty = false;
}

void RenderViewParameterData::Init()
{
	mCbufferDesc = sRenderResourceContext->GetDefaultShaderConstantBufferDesc(LString("ViewBuffer").Hash());
	sGlobelRhiResourceGenerator->GetDeviceUniformObjectGenerator()->CreateUniformBufferAndView(mCbufferDesc, mViewParamRes, mViewParamCbufferView);
	mViewCbuffer = MakeShared<graphics::ShaderCBuffer>(GetParamDesc());
}

RenderView::RenderView() :
	mRT(nullptr)
{	

}

void RenderView::Culling(RenderScene* scene)
{
	scene->GetRenderObjects(mViewVisibleROs);
}

void RenderView::Render(FrameGraphBuilder* builder)
{
	if(mViewType == RenderViewType::SceneView)
	{
		SceneViewPipeline(builder, mOwnerScene, this);
	}
	if(mViewType == RenderViewType::ShadowMapView)
	{
		ShadowCastViewPipeline(builder, mOwnerScene, this);
	}
}

void RenderViewDataGenerateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData)
{
	std::function<void(void)> commandFunc = [=, &renderData]()->void {
		renderData.mView = curScene->CreateRenderView();
		renderData.mView->mViewType = graphics::RenderViewType::SceneView;
		renderData.mViewParamData = renderData.mView->RequireData<graphics::RenderViewParameterData>();
		renderData.mViewParamData->Init();
	};
	curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_GENERATE, commandFunc);
}

void RenderViewDataRenderTargetUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, graphics::RenderTarget* renderTarget)
{
	std::function<void(void)> commandFunc = [=, &renderData]()->void {
		renderData.mView->SetRenderTarget(renderTarget);
	};
	curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
}

void RenderViewDataTargetWidthUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, int32_t width)
{
	std::function<void(void)> commandFunc = [=, &renderData]()->void {
		renderData.mViewParamData->mIntrinsicsParameter.mRtWidth = width;
		renderData.mViewParamData->MarkIntrinsicsDirty();
	};
	curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
}

void RenderViewDataTargetHeightUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, int32_t height)
{
	std::function<void(void)> commandFunc = [=, &renderData]()->void {
		renderData.mViewParamData->mIntrinsicsParameter.mRtHeight = height;
		renderData.mViewParamData->MarkIntrinsicsDirty();
	};
	curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
}

void RenderViewDataFovUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, float fovY)
{
	std::function<void(void)> commandFunc = [=, &renderData]()->void {
		renderData.mViewParamData->mIntrinsicsParameter.mFovY = fovY;
		renderData.mViewParamData->MarkIntrinsicsDirty();
	};
	curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
}

void RenderViewDataNearPlaneUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, float nearValue)
{
	std::function<void(void)> commandFunc = [=, &renderData]()->void {
		renderData.mViewParamData->mIntrinsicsParameter.mNear = nearValue;
		renderData.mViewParamData->MarkIntrinsicsDirty();
	};
	curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
}

void RenderViewDataFarPlaneUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, float farValue)
{
	std::function<void(void)> commandFunc = [=, &renderData]()->void {
		renderData.mViewParamData->mIntrinsicsParameter.mFar = farValue;
		renderData.mViewParamData->MarkIntrinsicsDirty();
	};
	curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
}

void RenderViewDataPositionUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, const LVector3f& position)
{
	std::function<void(void)> commandFunc = [=, &renderData]()->void {
		renderData.mViewParamData->mExtrinsicsParameter.mPosition = position;
		renderData.mViewParamData->MarkExtrinsicsDirty();
	};
	curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
}

void RenderViewDataViewMatrixUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, const LMatrix4f& vieMat)
{
	std::function<void(void)> commandFunc = [=, &renderData]()->void {
		renderData.mViewParamData->mExtrinsicsParameter.mViewMatrix = vieMat;
		renderData.mViewParamData->MarkExtrinsicsDirty();
	};
	curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
}

}
