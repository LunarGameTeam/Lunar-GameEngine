#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/Renderer/RenderContext.h"
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
		RHICBufferDesc mCbufferDesc = sGlobelRenderResourceContext->GetDefaultShaderConstantBufferDesc(LString("PointBasedLightParameter").Hash());
		if (mCbufferDesc.mSize == 0)
		{
			return;
		}
		mLightBufferGlobelMessage = MakeShared<ShaderCBuffer>(mCbufferDesc);
		sGlobelRhiResourceGenerator->GetDeviceUniformObjectGenerator()->CreateUniformBufferAndView(mCbufferDesc, mLightParameterBuffer, mLightParameterBufferView);
		//��Դ���ݵ�buffer
		size_t elementSize = 4 * sizeof(LVector4f);
		RHIBufferDesc dataBufferDesc;
		dataBufferDesc.mBufferUsage = RHIBufferUsage::StructureBuffer;
		dataBufferDesc.mSize = elementSize * 128;
		mExistLightDataBuffer = sGlobelRhiResourceGenerator->GetDeviceResourceGenerator()->CreateBuffer(RHIHeapType::Default, dataBufferDesc);

		ViewDesc dataBufferviewDesc;
		dataBufferviewDesc.mViewType = RHIViewType::kStructuredBuffer;
		dataBufferviewDesc.mViewDimension = RHIViewDimension::BufferView;
		dataBufferviewDesc.mStructureStride = sizeof(LVector4f);
		mExistLightDataBufferView = sGlobelRenderDevice->CreateView(dataBufferviewDesc);
		mExistLightDataBufferView->BindResource(mExistLightDataBuffer);

		mMaterial = sAssetModule->LoadAsset<MaterialComputeAsset>("/assets/built-in/LightDataCopy.mat");
		mMaterialInstance = LSharedPtr<MaterialInstanceComputeBase>(dynamic_cast<MaterialInstanceComputeBase*>(mMaterial->CreateInstance()));
		LightDataInitSucceed = true;
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
		if (!LightDataInitSucceed || !LightNumDirty)
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
		if (!LightDataInitSucceed || mDirtyList.size() == 0)
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

	void PointBasedRenderLightData::MarkLightDirty(PointBasedLight* light)
	{
		if (mDirtyList.find(light) == mDirtyList.end())
		{
			mDirtyList.insert(light);
		}
	};

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

	void RenderLightDataGenerateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, PointBasedLightType type)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			renderData.mRenderData = curScene->RequireData<graphics::PointBasedRenderLightData>();
			renderData.mRenderLight = renderData.mRenderData->CreatePointBasedLight();
			renderData.mRenderLight->mType = type;
			renderData.mRenderData->MarkLightDirty(renderData.mRenderLight);
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_GENERATE,commandFunc);
	}

	void GenerateRenderLightColorUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const LVector4f& colorIn)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			renderData.mRenderLight->mColor = colorIn;
			renderData.mRenderData->MarkLightDirty(renderData.mRenderLight);
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}

	void GenerateRenderLightIntensityUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const float intensity)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			renderData.mRenderLight->mIntensity = intensity;
			renderData.mRenderData->MarkLightDirty(renderData.mRenderLight);
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}

	void GenerateRenderLightPositionUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const LVector3f& position)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			renderData.mRenderLight->mPosition = position;
			renderData.mRenderData->MarkLightDirty(renderData.mRenderLight);
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}

	void GenerateRenderLightDirectionUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const LVector3f& dir)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			renderData.mRenderLight->mDirection = dir;
			renderData.mRenderData->MarkLightDirty(renderData.mRenderLight);
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}

	void GenerateRenderLightRangeParameterUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const LVector4f& rangeParamter)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			renderData.mRenderLight->mRangeParam = rangeParamter;
			renderData.mRenderData->MarkLightDirty(renderData.mRenderLight);
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}

	void GenerateRenderCastShadowUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const bool castShadow)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			if (renderData.mView == nullptr && castShadow == true)
			{
				renderData.mView = curScene->CreateRenderView();
				renderData.mView->mViewType = graphics::RenderViewType::ShadowMapView;
			}
			if (renderData.mView != nullptr && castShadow == false)
			{
				curScene->DestroyRenderView(renderData.mView);
				renderData.mView = nullptr;
			}
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}
}