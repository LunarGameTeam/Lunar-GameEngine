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
	void PointBasedRenderLightData::SetMaterialParameter(MaterialInstance* matInstance)
	{
		matInstance->SetShaderInput(ParamID_PointBasedLightParameter, mLightParameterBufferView);
		matInstance->SetShaderInput(ParamID_PointBasedLightDataBUffer, mExistLightDataBufferView);
	}

	PointBasedRenderLightData::PointBasedRenderLightData()
	{
		mPointLightIndex.resize(256);

		mDirectionLightIndex.resize(16);

		mSpotLightIndex.resize(256);
		//光源参数的cbuffer
		auto device = sRenderModule->GetRenderContext();
		RHICBufferDesc mCbufferDesc = device->GetDefaultShaderConstantBufferDesc(LString("PointBasedLightParameter").Hash());
		mLightBufferGlobelMessage = MakeShared<ShaderCBuffer>(mCbufferDesc);
		RHIBufferDesc desc;
		desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
		desc.mSize = SizeAligned2Pow(mCbufferDesc.mSize,CommonSize64K);
		ViewDesc viewDesc;
		viewDesc.mViewType = RHIViewType::kConstantBuffer;
		viewDesc.mViewDimension = RHIViewDimension::BufferView;
		mLightParameterBuffer = sRenderModule->GetRenderContext()->CreateBuffer(RHIHeapType::Default, desc);
		mLightParameterBufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
		mLightParameterBufferView->BindResource(mLightParameterBuffer);
		//光源数据的buffer
		size_t elementSize = 4 * sizeof(LVector4f);
		RHIBufferDesc desc;
		desc.mBufferUsage = RHIBufferUsage::StructureBuffer;
		desc.mSize = elementSize * 128;
		mExistLightDataBuffer = sRenderModule->GetRenderContext()->CreateBuffer(RHIHeapType::Default, desc);

		ViewDesc viewDesc;
		viewDesc.mViewType = RHIViewType::kStructuredBuffer;
		viewDesc.mViewDimension = RHIViewDimension::BufferView;
		viewDesc.mStructureStride = sizeof(LVector4f);
		mExistLightDataBufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
		mExistLightDataBufferView->BindResource(mExistLightDataBuffer);
	}

	PointBasedLight* PointBasedRenderLightData::CreatePointBasedLight()
	{
		LightNumDirty = true;
		return mAllLights.AddNewValue();
	}

	void PointBasedRenderLightData::DestroyLight(PointBasedLight* light)
	{
		LightNumDirty = true;
		mAllLights.DestroyValue(light);
	}

	void PointBasedRenderLightData::PerSceneUpdate(RenderScene* renderScene)
	{
		if (LightNumDirty)
		{
			//如果光源数量发生变化，需要更新一下cbuffer里的参数
			LArray<PointBasedLight*> allLightData;
			mAllLights.GetAllValueList(allLightData);
			int32_t pointLightSize = 0;
			int32_t directionLightSize = 0;
			int32_t spotLightSize = 0;
			for (auto eachLight : allLightData)
			{
				switch (eachLight->mType)
				{
				case PointBasedLightType::POINT_BASED_LIGHT_POINT:
				{
					mPointLightIndex[pointLightSize] = eachLight->mIndex;
					pointLightSize += 1;
					break;
				}
				case PointBasedLightType::POINT_BASEDLIGHT_DIRECTION:
				{
					mDirectionLightIndex[directionLightSize] = eachLight->mIndex;
					directionLightSize += 1;
					break;
				}
				case PointBasedLightType::POINT_BASEDLIGHT_SPOT:
				{
					mSpotLightIndex[spotLightSize] = eachLight->mIndex;
					spotLightSize += 1;
					break;
				}
				default:
					break;
				}
			}
			LVector4i lightNum;
			lightNum.x() = pointLightSize;
			lightNum.y() = directionLightSize;
			lightNum.z() = spotLightSize;
			lightNum.w() = 0;
			mLightBufferGlobelMessage->Set("pointBasedLightNum", lightNum);
			mLightBufferGlobelMessage->SetData("cPointLightIndex", mPointLightIndex.data(),256 * sizeof(int32_t));
			mLightBufferGlobelMessage->SetData("cDirectionLightIndex", mDirectionLightIndex.data(), 16 * sizeof(int32_t));
			mLightBufferGlobelMessage->SetData("cSpotLightIndex", mSpotLightIndex.data(), 256 * sizeof(int32_t));
			RHIResource* lightParamBuffer = renderScene->GetStageBufferPool()->AllocUniformStageBuffer(mLightBufferGlobelMessage.get());

			graphics::GpuSceneUploadCopyCommand* copyCommand = renderScene->AddCopyCommand();
			copyCommand->mSrcOffset = 0;
			copyCommand->mDstOffset = 0;
			copyCommand->mCopyLength = SizeAligned2Pow(mLightBufferGlobelMessage->mData.size(), 256);
			copyCommand->mUniformBufferInput = lightParamBuffer;
			copyCommand->mStorageBufferOutput = mLightParameterBuffer.get();

			LightNumDirty = false;
		}
		if (mDirtyList.size() == 0)
		{
			return;
		}
		LArray<LVector4f> lightUpdateData;
		LArray<uint32_t> lightUpdateIndex;

		RHIResource* lightDataBuffer = renderScene->GetStageBufferPool()->AllocStructStageBuffer(mDirtyList.size() * sizeof(LVector4f) * 4);
		LVector4f* currentLightDataPointer = (LVector4f*)lightDataBuffer->Map();
		RHIResource* lightIndexBuffer = renderScene->GetStageBufferPool()->AllocStructStageBuffer(mDirtyList.size() * sizeof(uint32_t));
		uint32_t* currentLightIndexPointer = (uint32_t*)lightIndexBuffer->Map();
		//index的最前面存储尺寸
		*currentLightIndexPointer = (uint32_t)mDirtyList.size();
		currentLightIndexPointer += 1;
		for (auto eachDirtyLight : mDirtyList)
		{
			//光源颜色
			LVector4f& lightColor = currentLightDataPointer[0];
			lightColor = eachDirtyLight->mColor;
			//范围及衰减参数
			LVector4f& rangeParam = currentLightDataPointer[1];
			rangeParam = eachDirtyLight->mRangeParam;
			//位置和颜色强度
			LVector4f& positionIntensity = currentLightDataPointer[2];
			positionIntensity.x() = eachDirtyLight->mPosition.x();
			positionIntensity.y() = eachDirtyLight->mPosition.y();
			positionIntensity.z() = eachDirtyLight->mPosition.z();
			positionIntensity.w() = eachDirtyLight->mIntensity;
			//方向和聚光参数
			LVector4f& directionParam = currentLightDataPointer[3];
			directionParam.x() = eachDirtyLight->mDirection.x();
			directionParam.y() = eachDirtyLight->mDirection.y();
			directionParam.z() = eachDirtyLight->mDirection.z();
			directionParam.w() = 0;

			currentLightDataPointer += 4;
			//光源ID
			*currentLightIndexPointer = (uint32_t)eachDirtyLight->mIndex;
			currentLightIndexPointer += 1;
		}
		lightDataBuffer->Unmap();
		lightIndexBuffer->Unmap();
		graphics::GpuSceneUploadComputeCommand* computeCommand = renderScene->AddComputeCommand();
		size_t fullThreadSize = SizeAligned2Pow(mDirtyList.size(), 64);
		computeCommand->mDispatchSize.x() = fullThreadSize / 64;
		computeCommand->mDispatchSize.y() = 1;
		computeCommand->mDispatchSize.z() = 1;
		computeCommand->mStorageBufferInput.push_back(lightDataBuffer);
		computeCommand->mStorageBufferInput.push_back(lightIndexBuffer);
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