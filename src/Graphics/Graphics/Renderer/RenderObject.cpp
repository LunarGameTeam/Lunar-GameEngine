#include "Graphics/Renderer/RenderObject.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Graphics/Asset/MaterialTemplate.h"
#include "Graphics/RenderModule.h"
#include "Core/Asset/AssetModule.h"
namespace luna::graphics
{
	RenderObject::RenderObject()
	{
		mWorldMat.setIdentity();
	}

	PARAM_ID(RoWorldMatrixUploadBuffer);
	PARAM_ID(RoWorldMatrixIndexBuffer);
	PARAM_ID(RoWorldMatrixDataBuffer);
	RenderObjectDrawData::RenderObjectDrawData()
	{
		//Ro数据的buffer
		size_t elementSize = sizeof(LMatrix4f);
		RHIBufferDesc desc;
		desc.mBufferUsage = RHIBufferUsage::StructureBuffer;
		desc.mSize = elementSize * 4096;
		mRoMessagePtr = sRenderModule->GetRenderContext()->CreateBuffer(RHIHeapType::Default, desc);

		ViewDesc viewDesc;
		viewDesc.mViewType = RHIViewType::kRWStructuredBuffer;
		viewDesc.mViewDimension = RHIViewDimension::BufferView;
		viewDesc.mStructureStride = sizeof(LMatrix4f);
		mRoMessageView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
		mRoMessageView->BindResource(mRoMessagePtr);

		mMaterial = sAssetModule->LoadAsset<MaterialComputeAsset>("/assets/built-in/RoDataCopy.mat");
		mMaterialInstance = LSharedPtr<MaterialInstanceComputeBase>(dynamic_cast<MaterialInstanceComputeBase*>(mMaterial->CreateInstance()));
	}

	void RenderObjectDrawData::SetMaterialParameter(MaterialInstanceBase* matInstance)
	{
		matInstance->SetShaderInput(ParamID_RoWorldMatrixDataBuffer, mRoMessageView);
	}

	void RenderObjectDrawData::AddDirtyRo(RenderObject* roData)
	{
		dirtyRo.push_back(roData);
	}

	void RenderObjectDrawData::PerSceneUpdate(RenderScene* renderScene)
	{
		if (dirtyRo.size() == 0)
		{
			return;
		}
		//制作数据更新指令
		RHIView* RoDataBuffer = renderScene->GetStageBufferPool()->AllocStructStageBuffer(
			dirtyRo.size() * sizeof(LMatrix4f) * 4,
			RHIViewType::kStructuredBuffer,
			sizeof(LMatrix4f),
			std::bind(&RenderObjectDrawData::GenerateRoMatrixBuffer, this, std::placeholders::_1)
		);

		RHIView* RoIndexBuffer = renderScene->GetStageBufferPool()->AllocStructStageBuffer(
			dirtyRo.size() * sizeof(uint32_t),
			RHIViewType::kStructuredBuffer,
			sizeof(uint32_t),
			std::bind(&RenderObjectDrawData::GenerateRoIndexBuffer, this, std::placeholders::_1)
		);

		graphics::GpuSceneUploadComputeCommand* computeCommand = renderScene->AddComputeCommand();
		size_t fullThreadSize = SizeAligned2Pow(dirtyRo.size(), 64);
		computeCommand->mDispatchSize.x() = fullThreadSize / 64;
		computeCommand->mDispatchSize.y() = 1;
		computeCommand->mDispatchSize.z() = 1;
		computeCommand->mStorageBufferInput.insert({ ParamID_RoWorldMatrixUploadBuffer, RoDataBuffer });
		computeCommand->mStorageBufferInput.insert({ ParamID_RoWorldMatrixIndexBuffer, RoIndexBuffer });
		computeCommand->mStorageBufferOutput.insert({ ParamID_RoWorldMatrixDataBuffer,mRoMessageView.get() });
		computeCommand->mComputeMaterial = mMaterialInstance.get();
		//临时代码，用于debug
		//dirtyRo.clear();
	}

	void RenderObjectDrawData::GenerateRoMatrixBuffer(void* pointer)
	{
		LMatrix4f* currentRoDataPointer = (LMatrix4f*)pointer;
		for (auto eachRo : dirtyRo)
		{
			*currentRoDataPointer = eachRo->GetWorldMatrix();
			currentRoDataPointer += 1;
		}
	}

	void RenderObjectDrawData::GenerateRoIndexBuffer(void* pointer)
	{
		uint32_t* currentRoIndexPointer = (uint32_t*)pointer;
		*currentRoIndexPointer = (uint32_t)dirtyRo.size();
		currentRoIndexPointer += 1;
		for (auto eachRo : dirtyRo)
		{
			*currentRoIndexPointer = (uint32_t)eachRo->mID;
			currentRoIndexPointer += 1;
		}
	}
}