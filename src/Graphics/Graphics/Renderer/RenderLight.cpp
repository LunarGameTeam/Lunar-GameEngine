#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Asset/MeshAsset.h"

namespace luna::graphics
{
	PARAM_ID(PointBasedLightParameter);
	PARAM_ID(PointBasedLightDataBUffer);
	void RenderLightGpuDataBuffer::SetMaterialParameter(MaterialInstance* matInstance)
	{
		matInstance->SetShaderInput(ParamID_PointBasedLightParameter, mLightParameter->mView);
		matInstance->SetShaderInput(ParamID_PointBasedLightDataBUffer, mExistLightDataBufferView);
	}

	PointBasedRenderLightData::PointBasedRenderLightData()
	{
		size_t elementSize = 3 * sizeof(LVector4f);
		RHIBufferDesc desc;
		desc.mBufferUsage = RHIBufferUsage::StructureBuffer;
		desc.mSize = elementSize * 128;
		mExistLightDataBuffer = sRenderModule->GetRenderContext()->CreateBuffer(RHIHeapType::Upload, desc);

		ViewDesc viewDesc;
		viewDesc.mViewType = RHIViewType::kStructuredBuffer;
		viewDesc.mViewDimension = RHIViewDimension::BufferView;
		viewDesc.mStructureStride = sizeof(LVector4f);
		mExistLightDataBufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
		mExistLightDataBufferView->BindResource(mExistLightDataBuffer);
	}

	PointBasedLight* PointBasedRenderLightData::CreatePointBasedLight()
	{
		return mAllLights.AddNewValue();
	}

	void PointBasedRenderLightData::DestroyLight(PointBasedLight* light)
	{
		mAllLights.DestroyValue(light);
	}

	void PointBasedRenderLightData::PerSceneUpdate(RenderScene* renderScene)
	{
		//这里先全量刷新，后面看能不能懒更新
		LVector4f* lightBufferData = (LVector4f*)mExistLightDataBuffer->Map();
		size_t mPointLightSize = 0;
		size_t mDirectionLightSize = 0;
		size_t mSpotLightSize = 0;
		for (auto eachLight : mAllLights)
		{
			switch (eachLight.second->mType)
			{
			case PointBasedLightType::POINT_BASED_LIGHT_POINT:
			{
				mPointLightSize += 1;
				break;
			}
			case PointBasedLightType::POINT_BASEDLIGHT_DIRECTION:
			{
				mDirectionLightSize += 1;
				break;
			}
			case PointBasedLightType::POINT_BASEDLIGHT_SPOT:
			{
				mSpotLightSize += 1;
				break;
			}
			default:
				break;
			}
		}
		size_t mPointLightCopy = 0;
		size_t mDirectionLightCopy = 0;
		size_t mSpotLightCopy = 0;
		for (auto eachLight : mAllLights)
		{
			LVector4f* currentPointer = nullptr;
			switch (eachLight.second->mType)
			{
			case PointBasedLightType::POINT_BASED_LIGHT_POINT:
			{
				currentPointer = lightBufferData + 3 * mPointLightCopy;
				mPointLightCopy += 1;
				break;
			}
			case PointBasedLightType::POINT_BASEDLIGHT_DIRECTION:
			{
				currentPointer = lightBufferData + 3 * (mPointLightSize + mDirectionLightCopy);
				currentPointer[2] = eachLight.second->mDirection;
				currentPointer[2].w() = 0;
				mDirectionLightCopy += 1;
				break;
			}
			case PointBasedLightType::POINT_BASEDLIGHT_SPOT:
			{
				currentPointer = lightBufferData + 3 * (mPointLightSize + mDirectionLightSize + mSpotLightCopy);
				mSpotLightCopy += 1;
				break;
			}
			default:
				break;
			}
			currentPointer[0] = eachLight.second->mColor;
			currentPointer[1].x() = eachLight.second->mIntensity;
		}
		mExistLightDataBuffer->Unmap();
		LVector4i lightNum;
		lightNum.x() = (int32_t)mPointLightSize;
		lightNum.y() = (int32_t)mDirectionLightSize;
		lightNum.z() = (int32_t)mSpotLightSize;
		lightNum.w() = (int32_t)0;
		mLightBufferGlobelMessage->Set("pointBasedLightNum", lightNum);
		mLightBufferGlobelMessage->Commit();

	}
//void DirectionLight::PerViewUpdate(RenderView* view)
//{
//	
//	if (!mParamBuffer)
//		mParamBuffer = new ShaderCBuffer(sRenderModule->GetRenderContext()->GetDefaultShaderConstantBufferDesc(LString("ViewBuffer").Hash()));
//	LTransform transform = LTransform::Identity();
//	auto rota = LQuaternion::FromTwoVectors(LVector3f(0, 0, 1), mDirection);
//	transform.rotate(rota);
//	mViewMatrix = transform.matrix().inverse();
//	LMatrix4f proj;
//	LMath::GenOrthoLHMatrix(proj, 30, 30, 0.01, 50);
//	mParamBuffer->Set("cViewMatrix", mViewMatrix);
//	mParamBuffer->Set("cProjectionMatrix", proj);
//	view->mOwnerScene->mSceneParamsBuffer->Set("cDirectionLightViewMatrix", mViewMatrix, 0);
//	view->mOwnerScene->mSceneParamsBuffer->Set("cDirectionLightProjMatrix", proj, 0);
//	mParamBuffer->Commit();
//}
//
//void PointLight::PerViewUpdate(RenderView* view)
//{
//	if (mCastShadow)
//	{
//		if (mParamBuffer.size() == 0)
//		{
//			const auto& desc = sRenderModule->GetRenderContext()->GetDefaultShaderConstantBufferDesc(LString("ViewBuffer").Hash());
//			mParamBuffer.push_back(new ShaderCBuffer(desc));
//			mParamBuffer.push_back(new ShaderCBuffer(desc));
//			mParamBuffer.push_back(new ShaderCBuffer(desc));
//			mParamBuffer.push_back(new ShaderCBuffer(desc));
//			mParamBuffer.push_back(new ShaderCBuffer(desc));
//			mParamBuffer.push_back(new ShaderCBuffer(desc));
//			mViewMatrix.resize(6);
//		}
//		LQuaternion rotation[] =
//		{
//			LMath::FromEuler(LVector3f(0,0,0)),
//			LMath::FromEuler(LVector3f(0,90,0)),
//			LMath::FromEuler(LVector3f(0,180,0)),
//			LMath::FromEuler(LVector3f(0,270,0)),
//			LMath::FromEuler(LVector3f(90,0,0)),
//			LMath::FromEuler(LVector3f(-90,0,0)),
//		};
//
//		LMath::GenPerspectiveFovLHMatrix(mProjMatrix, mFov, mAspect, mNear, mFar);
//
//		for (uint32_t idx = 0; idx < 6; idx++)
//		{
//			LTransform transform = LTransform::Identity();
//			transform.translate(mPosition);
//			transform.rotate(rotation[idx]);
//			mViewMatrix[idx] = transform.matrix().inverse();
//			mParamBuffer[idx]->Set("cViewMatrix", mViewMatrix[idx]);
//			mParamBuffer[idx]->Set("cProjectionMatrix", mProjMatrix);
//			view->mOwnerScene->mSceneParamsBuffer->Set("cLightViewMatrix", mViewMatrix[idx], idx);
//			view->mOwnerScene->mSceneParamsBuffer->Set("cLightProjMatrix", mProjMatrix, idx);
//		}
//		for (uint32_t idx = 0; idx < 6; idx++)
//		{
//			mParamBuffer[idx]->Commit();
//		}
//	}
//}

}