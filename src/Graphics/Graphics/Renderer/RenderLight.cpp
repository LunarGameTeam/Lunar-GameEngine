#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Core/Asset/AssetModule.h"

namespace luna::graphics
{
	PARAM_ID(PointBasedLightParameter);
	PARAM_ID(PointBasedLightDataBUffer);
	void PointBasedRenderLightData::SetMaterialParameter(MaterialInstance* matInstance)
	{
		matInstance->SetShaderInput(ParamID_PointBasedLightParameter, mLightParameterBufferView);
		matInstance->SetShaderInput(ParamID_PointBasedLightDataBUffer, mExistLightDataBufferView);
	}

	PointBasedRenderLightData::PointBasedRenderLightData():mMaterial(nullptr)
	{
		mPointLightIndex.resize(256);

		mDirectionLightIndex.resize(16);

		mSpotLightIndex.resize(256);
		//��Դ������cbuffer
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
		//��Դ���ݵ�buffer
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

		mMaterial = sAssetModule->LoadAsset<MaterialComputeAsset>("/assets/built-in/LightDataCopy.mat");
		mMaterialInstance = LSharedPtr<MaterialInstanceComputeBase>(dynamic_cast<MaterialInstanceComputeBase*>(mMaterial->CreateInstance()));
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
	PARAM_ID(PointBasedLightUploadBUffer);
	PARAM_ID(PointBasedLightIndexBUffer);
	
	void PointBasedRenderLightData::UpdateLightNumParameter(RenderScene* renderScene)
	{
		if (!LightNumDirty)
		{
			return;
		}
		//�����Դ���������仯����Ҫ����һ��cbuffer��Ĳ���
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
		mLightBufferGlobelMessage->SetData("cPointLightIndex", mPointLightIndex.data(), 256 * sizeof(int32_t));
		mLightBufferGlobelMessage->SetData("cDirectionLightIndex", mDirectionLightIndex.data(), 16 * sizeof(int32_t));
		mLightBufferGlobelMessage->SetData("cSpotLightIndex", mSpotLightIndex.data(), 256 * sizeof(int32_t));
		//cbuffer�ĸ���ֻ��Ҫ����һ��copy��ָ��
		renderScene->AddCbufferCopyCommand(mLightBufferGlobelMessage.get(), mLightParameterBuffer.get());
		LightNumDirty = false;
	}

	void PointBasedRenderLightData::UpdateDirtyLightData(RenderScene* renderScene)
	{
		if (mDirtyList.size() == 0)
		{
			return;
		}
		//����в��ֹ�Դ�����ݷ����˱仯����dirty�Ĺ�Դ���ݣ�ͨ������cs����ָ����³�����Ĺ�Դbuffer
		RHIView* lightDataBuffer = renderScene->GetStageBufferPool()->AllocStructStageBuffer(
			mDirtyList.size() * sizeof(LVector4f) * 4,
			RHIViewType::kStructuredBuffer,
			sizeof(LVector4f),
			std::bind(&PointBasedRenderLightData::GenerateDirtyLightDataBuffer, this, std::placeholders::_1)
		);

		RHIView* lightIndexBuffer = renderScene->GetStageBufferPool()->AllocStructStageBuffer(
			mDirtyList.size() * sizeof(uint32_t),
			RHIViewType::kStructuredBuffer,
			sizeof(uint32_t),
			std::bind(&PointBasedRenderLightData::GenerateDirtyLightIndexBuffer, this, std::placeholders::_1)
		);

		graphics::GpuSceneUploadComputeCommand* computeCommand = renderScene->AddComputeCommand();
		size_t fullThreadSize = SizeAligned2Pow(mDirtyList.size(), 64);
		computeCommand->mDispatchSize.x() = fullThreadSize / 64;
		computeCommand->mDispatchSize.y() = 1;
		computeCommand->mDispatchSize.z() = 1;
		computeCommand->mStorageBufferInput.insert({ ParamID_PointBasedLightUploadBUffer, lightDataBuffer });
		computeCommand->mStorageBufferInput.insert({ ParamID_PointBasedLightIndexBUffer, lightIndexBuffer });
		computeCommand->mStorageBufferOutput.insert({ ParamID_PointBasedLightDataBUffer,mExistLightDataBufferView.get() });
		computeCommand->mComputeMaterial = mMaterialInstance.get();
		mDirtyList.clear();
	}

	void PointBasedRenderLightData::GenerateDirtyLightDataBuffer(void* pointer)
	{
		LVector4f* currentLightDataPointer = (LVector4f*)pointer;
		for (auto eachDirtyLight : mDirtyList)
		{
			//��Դ��ɫ
			LVector4f& lightColor = currentLightDataPointer[0];
			lightColor = eachDirtyLight->mColor;
			//��Χ��˥������
			LVector4f& rangeParam = currentLightDataPointer[1];
			rangeParam = eachDirtyLight->mRangeParam;
			//λ�ú���ɫǿ��
			LVector4f& positionIntensity = currentLightDataPointer[2];
			positionIntensity.x() = eachDirtyLight->mPosition.x();
			positionIntensity.y() = eachDirtyLight->mPosition.y();
			positionIntensity.z() = eachDirtyLight->mPosition.z();
			positionIntensity.w() = eachDirtyLight->mIntensity;
			//����;۹����
			LVector4f& directionParam = currentLightDataPointer[3];
			directionParam.x() = eachDirtyLight->mDirection.x();
			directionParam.y() = eachDirtyLight->mDirection.y();
			directionParam.z() = eachDirtyLight->mDirection.z();
			directionParam.w() = 0;
			currentLightDataPointer += 4;
		}
	}

	void PointBasedRenderLightData::GenerateDirtyLightIndexBuffer(void* pointer)
	{
		uint32_t* currentLightIndexPointer = (uint32_t*)pointer;
		//index����ǰ��洢�ߴ�
		*currentLightIndexPointer = (uint32_t)mDirtyList.size();
		currentLightIndexPointer += 1;
		for (auto eachDirtyLight : mDirtyList)
		{
			//��ԴID
			*currentLightIndexPointer = (uint32_t)eachDirtyLight->mIndex;
			currentLightIndexPointer += 1;
		}
	}

	void PointBasedRenderLightData::PerSceneUpdate(RenderScene* renderScene)
	{
		UpdateLightNumParameter(renderScene);

		UpdateDirtyLightData(renderScene);
	}
}