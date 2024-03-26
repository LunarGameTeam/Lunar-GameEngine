#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Graphics/Asset/MeshAssetUtils.h"
#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/MaterialParam.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/FrameGraph/FrameGraph.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/SkeletonSkin.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/Renderer/RenderContext.h"
namespace luna::graphics
{

RenderSceneStagingMemory::RenderSceneStagingMemory(size_t perStageSize) : mPerStageSize(perStageSize)
{
	AddNewMemory(perStageSize);
	mPoolUsedSize = 0;
}

RHIView* RenderSceneStagingMemory::AllocStructStageBuffer(size_t curBufferSize, RHIViewType useType, size_t strideSize)
{
	//目前不考虑多线程访问的问题，默认render只有一个线程
	if (mPoolUsedSize <= mStageBufferPool.size())
	{
		RHIBufferDesc newBufferDesc;
		newBufferDesc.mBufferUsage = RHIBufferUsage::TransferSrcBit | RHIBufferUsage::StructureBuffer;
		newBufferDesc.mSize = SizeAligned2Pow(curBufferSize, CommonSize64K);
		RHIResourcePtr newBuffer = sGlobelRenderDevice->CreateBufferExt(newBufferDesc);
		mStageBufferPool.push_back(newBuffer);

		ViewDesc viewDesc;
		viewDesc.mViewType = useType;
		viewDesc.mViewDimension = RHIViewDimension::BufferView;
		viewDesc.mStructureStride = strideSize;
		RHIViewPtr newBufferView = sGlobelRenderDevice->CreateView(viewDesc);
		mStageBufferViewPool.push_back(newBufferView);
		newBufferView->BindResource(newBuffer);
	}
	RHIResource* curResourceItm = nullptr;
	curResourceItm = mStageBufferPool[mPoolUsedSize].get();
	RHIView* newBufferView = nullptr;
	newBufferView = mStageBufferViewPool[mPoolUsedSize].get();
	BindMemory(curResourceItm);
	newBufferView->BindResource(curResourceItm);
	mPoolUsedSize += 1;
	return newBufferView;
};

RHIView* RenderSceneStagingMemory::AllocStructStageBuffer(size_t curBufferSize, RHIViewType useType, size_t strideSize, std::function<void(void*)> memoryFunc)
{
	RHIView* curView = AllocStructStageBuffer(curBufferSize, useType, strideSize);
	void* curPointer = curView->mBindResource->Map();
	memoryFunc(curPointer);
	curView->mBindResource->Unmap();
	return curView;
}

RHIView* RenderSceneStagingMemory::AllocUniformStageBuffer(ShaderCBuffer* cbufferData)
{
	//这里需要后期处理下cbuffer的分配，目前每个cbuffer分配64k会造成巨大的浪费
	RHIView* newRes = AllocStructStageBuffer(cbufferData->mData.size(), RHIViewType::kConstantBuffer,0);
	void* cpuPointer = newRes->mBindResource->Map();
	memcpy(cpuPointer, cbufferData->mData.data(), cbufferData->mData.size());
	newRes->mBindResource->Unmap();
	return newRes;
}

void RenderSceneStagingMemory::AddNewMemory(size_t memorySize)
{
	RHIMemoryPtr curMemoryData;
	RHIMemoryDesc memoryDesc;
	memoryDesc.SizeInBytes = memorySize;
	memoryDesc.Type = RHIHeapType::Upload;
	mMemoryOffset = 0;
	curMemoryData = sGlobelRenderDevice->AllocMemory(memoryDesc);
	mMemoryData.push_back(curMemoryData);
	mMemoryDataSize.push_back(memorySize);
}

bool RenderSceneStagingMemory::BindMemory(RHIResource* targetResource)
{
	size_t mMemorySize = mMemoryDataSize.back();
	size_t reauiredSize = SizeAligned2Pow(targetResource->GetMemoryRequirements().size, CommonSize64K);
	if ((mMemoryOffset + reauiredSize + CommonSize64K) >= mMemorySize)
	{
		if (reauiredSize * 4 >= mPerStageSize)
		{
			size_t nextBufferSize = SizeAligned2Pow(reauiredSize * 4, CommonSize64K);
			AddNewMemory(nextBufferSize);
		}
		else
		{
			AddNewMemory(mPerStageSize);
		}
		mMemoryOffset = 0;
	}
	targetResource->BindMemory(mMemoryData.back().get(), mMemoryOffset);
	mMemoryOffset += reauiredSize;
	return true;
}

void RenderSceneStagingMemory::ClearMemory()
{
	if (mMemoryData.size() == 1)
	{
		mMemoryOffset = 0;
		return;
	}
	size_t nextFrameSize = 0;
	for (int32_t memoryIndex = 0; memoryIndex < mMemoryDataSize.size() - 1; ++memoryIndex)
	{
		nextFrameSize += mMemoryDataSize[memoryIndex];
	}
	nextFrameSize += mMemoryOffset;
	nextFrameSize += 0x400000;
	mMemoryOffset = 0;
	mMemoryData.clear();
	mMemoryDataSize.clear();
	AddNewMemory(nextFrameSize);
};

RenderSceneUploadBufferPool::RenderSceneUploadBufferPool()
{
	mFrameIndex = 0;
	for (int32_t i = 0; i < 2; ++i)
	{
		mMemoryArray.emplace_back();
	}

}

RHIView* RenderSceneUploadBufferPool::AllocStructStageBuffer(size_t curBufferSize, RHIViewType useType, size_t strideSize)
{
	return mMemoryArray[mFrameIndex].AllocStructStageBuffer(curBufferSize, useType, strideSize);
}

RHIView* RenderSceneUploadBufferPool::AllocStructStageBuffer(size_t curBufferSize, RHIViewType useType, size_t strideSize, std::function<void(void*)> memoryFunc)
{
	return mMemoryArray[mFrameIndex].AllocStructStageBuffer(curBufferSize, useType, strideSize, memoryFunc);
}

RHIView* RenderSceneUploadBufferPool::AllocUniformStageBuffer(ShaderCBuffer* cbufferData)
{
	return mMemoryArray[mFrameIndex].AllocUniformStageBuffer(cbufferData);
}

void RenderSceneUploadBufferPool::Present()
{
	mFrameIndex = (mFrameIndex + 1) % 2;
}

void RenderDataUpdateCommandPool::AddCommand(RenderDataUpdateCommandType commandType, const std::function<void(void)>& func)
{
	auto& newValue = commandPool.emplace_back();
	newValue.mType = commandType;
	newValue.mExcuteFunc = func;
}

void RenderDataUpdateCommandPool::ExcuteCommand()
{
	LArray<size_t> generateCommand;
	LArray<size_t> updateCommand;
	for (size_t index = 0;index < commandPool.size(); ++index)
	{
		switch (commandPool[index].mType)
		{
		case RenderDataUpdateCommandType::RENDER_DATA_GENERATE:
		{
			generateCommand.push_back(index);
		}
		break;
		case RenderDataUpdateCommandType::RENDER_DATA_UPDATE:
		{
			updateCommand.push_back(index);
		}
		break;
		default:
			break;
		}
	}
	for (size_t generateIndex : generateCommand)
	{
		commandPool[generateIndex].mExcuteFunc();
	}
	for (size_t updateIndex : updateCommand)
	{
		commandPool[updateIndex].mExcuteFunc();
	}
	commandPool.clear();
}

RenderDataUpdateCommandAllocator::RenderDataUpdateCommandAllocator()
{
	for (int32_t i = 0; i < 3; ++i)
	{
		mPools.emplace_back();
	}
}

void RenderDataUpdateCommandAllocator::AddCommand(RenderDataUpdateCommandType commandType, const std::function<void(void)>& func)
{
	mPools[mGameLocation].AddCommand(commandType,func);
}

void RenderDataUpdateCommandAllocator::ExcuteCommand()
{
	mPools[mRenderLocation].ExcuteCommand();
	mRenderLocation = (mRenderLocation + 1) % 3;
}

void RenderDataUpdateCommandAllocator::FinishRecord()
{
	mGameLocation = (mGameLocation + 1) % 3;
}


PARAM_ID(RoWorldMatrixBuffer);
RenderScene::RenderScene()
{
	RequireData<SkeletonSkinData>();
	//if (mRoDataBuffer == nullptr)
	//{
	//	RHIBufferDesc desc;
	//	desc.mBufferUsage = RHIBufferUsage::StructureBuffer;
	//	desc.mSize = sizeof(LMatrix4f) * 1024;
	//	mRoDataBuffer = sRenderModule->mRenderContext->CreateBuffer(RHIHeapType::Upload, desc);
	//	ViewDesc viewDesc;
	//	viewDesc.mViewType = RHIViewType::kStructuredBuffer;
	//	viewDesc.mViewDimension = RHIViewDimension::BufferView;
	//	viewDesc.mStructureStride = sizeof(LMatrix4f);
	//	mRoDataBufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
	//	mRoDataBufferView->BindResource(mRoDataBuffer);
	//}
}

//void RenderScene::SetMaterialSceneParameter(MaterialInstance* matInstance)
//{
//	matInstance->SetShaderInput(ParamID_RoWorldMatrixBuffer, mRoDataBufferView);
//}

RenderObject* RenderScene::CreateRenderObject()
{
	RenderObject* newObject = mRenderObjects.AddNewValue();
	newObject->SetScene(this);
	return newObject;
}

void RenderScene::DestroyRenderObject(RenderObject* ro)
{
	mRenderObjects.DestroyValue(ro);
}

void RenderScene::GetRenderObjects(LArray<RenderObject*>& valueOut) const
{
	mRenderObjects.GetAllValueList(valueOut);
}

RenderView* RenderScene::CreateRenderView()
{
	RenderView* curView = mViews.AddNewValue();
	curView->mOwnerScene = this;
	return curView;
}

void RenderScene::GetAllView(LArray<RenderView*>& valueOut) const
{
	mViews.GetAllValueList(valueOut);
}

void RenderScene::DestroyRenderView(RenderView* renderView)
{
	mViews.DestroyValue(renderView);
}

GpuSceneUploadComputeCommand* RenderScene::AddComputeCommand()
{
	mAllComputeCommand.emplace_back();
	return &mAllComputeCommand.back();
}

GpuSceneUploadCopyCommand* RenderScene::AddCopyCommand()
{
	mAllCopyCommand.emplace_back();
	return &mAllCopyCommand.back();
}

void RenderScene::AddCbufferCopyCommand(ShaderCBuffer* cbufferData, RhiUniformBufferPack* bufferOutput)
{
	GpuSceneUploadCopyCommand* curCommand = AddCopyCommand();
	RHIResource* lightParamBuffer = mStageBufferPool.AllocUniformStageBuffer(cbufferData)->mBindResource;
	//cbuffer的更新只需要制作一个copy的指令
	assert(cbufferData->mData.size() == bufferOutput->GetSize());
	curCommand->mSrcOffset = 0;
	curCommand->mDstOffset = bufferOutput->GetOffset();
	curCommand->mCopyLength = cbufferData->mData.size();
	curCommand->mUniformBufferInput = lightParamBuffer;
	curCommand->mStorageBufferOutput = bufferOutput->GetRhiBuffer();
}

RenderSceneUploadBufferPool* RenderScene::GetStageBufferPool()
{
	return &mStageBufferPool;
}

void RenderScene::ExcuteCopy(RHICmdList* cmdList)
{
	LArray<ResourceBarrierDesc> curResourceBarrier;
	//修改资源状态
	for (auto& eachCommand : mAllComputeCommand)
	{
		for (auto& eachInput : eachCommand.mStorageBufferInput)
		{
			ResourceBarrierDesc newBarrierDesc;
			newBarrierDesc.mBarrierRes = eachInput.second->mBindResource;
			newBarrierDesc.mStateBefore = ResourceState::kGenericRead;
			newBarrierDesc.mStateAfter = ResourceState::kNonPixelShaderResource;
			curResourceBarrier.push_back(newBarrierDesc);
		}
		for (auto& eachOut : eachCommand.mStorageBufferOutput)
		{
			ResourceBarrierDesc newBarrierDesc;
			newBarrierDesc.mBarrierRes = eachOut.second->mBindResource;
			newBarrierDesc.mStateBefore = ResourceState::kNonPixelShaderResource;
			newBarrierDesc.mStateAfter = ResourceState::kUnorderedAccess;
			curResourceBarrier.push_back(newBarrierDesc);
		}
	}
	for (auto& eachCommand : mAllCopyCommand)
	{
		ResourceBarrierDesc newInputBarrierDesc;
		newInputBarrierDesc.mBarrierRes = eachCommand.mUniformBufferInput;
		newInputBarrierDesc.mStateBefore = ResourceState::kGenericRead;
		newInputBarrierDesc.mStateAfter = ResourceState::kCopySource;
		curResourceBarrier.push_back(newInputBarrierDesc);

		ResourceBarrierDesc newOutputBarrierDesc;
		newOutputBarrierDesc.mBarrierRes = eachCommand.mStorageBufferOutput;
		newOutputBarrierDesc.mStateBefore = ResourceState::kVertexAndConstantBuffer;
		newOutputBarrierDesc.mStateAfter = ResourceState::kCopyDest;
		curResourceBarrier.push_back(newInputBarrierDesc);
	}
	if(curResourceBarrier.size() != 0)
	{
		cmdList->ResourceBarrierExt(curResourceBarrier);
	}
	//执行GPUscene的更新指令
	for (auto& eachCommand : mAllComputeCommand)
	{
		for (auto eachStorageBuffer : eachCommand.mStorageBufferInput)
		{
			eachCommand.mComputeMaterial->SetShaderInput(eachStorageBuffer.first, eachStorageBuffer.second);
		}
		for (auto eachStorageBuffer : eachCommand.mStorageBufferOutput)
		{
			eachCommand.mComputeMaterial->SetShaderInput(eachStorageBuffer.first, eachStorageBuffer.second);
		}
		eachCommand.mComputeMaterial->UpdateBindingSet();
		sGlobelRenderCommondEncoder->Dispatch(cmdList,eachCommand.mComputeMaterial, eachCommand.mDispatchSize);
	}
	for (auto& eachCommand : mAllCopyCommand)
	{
		cmdList->CopyBufferToBuffer(eachCommand.mStorageBufferOutput, eachCommand.mDstOffset, eachCommand.mUniformBufferInput, eachCommand.mSrcOffset, eachCommand.mCopyLength);
	}
	//恢复资源状态
	curResourceBarrier.clear();
	for (auto& eachCommand : mAllComputeCommand)
	{
		for (auto& eachInput : eachCommand.mStorageBufferInput)
		{
			ResourceBarrierDesc newBarrierDesc;
			newBarrierDesc.mBarrierRes = eachInput.second->mBindResource;
			newBarrierDesc.mStateBefore = ResourceState::kNonPixelShaderResource;
			newBarrierDesc.mStateAfter = ResourceState::kGenericRead;
			curResourceBarrier.push_back(newBarrierDesc);
		}
		for (auto& eachOut : eachCommand.mStorageBufferOutput)
		{
			ResourceBarrierDesc newBarrierDesc;
			newBarrierDesc.mBarrierRes = eachOut.second->mBindResource;
			newBarrierDesc.mStateBefore = ResourceState::kUnorderedAccess;
			newBarrierDesc.mStateAfter = ResourceState::kNonPixelShaderResource;
			curResourceBarrier.push_back(newBarrierDesc);
		}
	}
	for (auto& eachCommand : mAllCopyCommand)
	{
		ResourceBarrierDesc newInputBarrierDesc;
		newInputBarrierDesc.mBarrierRes = eachCommand.mUniformBufferInput;
		newInputBarrierDesc.mStateBefore = ResourceState::kCopySource;
		newInputBarrierDesc.mStateAfter = ResourceState::kGenericRead;
		curResourceBarrier.push_back(newInputBarrierDesc);

		ResourceBarrierDesc newOutputBarrierDesc;
		newOutputBarrierDesc.mBarrierRes = eachCommand.mStorageBufferOutput;
		newOutputBarrierDesc.mStateBefore = ResourceState::kCopyDest;
		newOutputBarrierDesc.mStateAfter = ResourceState::kVertexAndConstantBuffer;
		curResourceBarrier.push_back(newInputBarrierDesc);
	}
	if (curResourceBarrier.size() != 0)
	{
		cmdList->ResourceBarrierExt(curResourceBarrier);
	}
	//清空所有GpuScene的更新指令
	mAllComputeCommand.clear();
	mAllCopyCommand.clear();
	//将上传缓冲池切换到下一帧
	mStageBufferPool.Present();
}

RenderScene::~RenderScene()
{
}

//void RenderScene::Debug()
//{
//	if (!mDrawGizmos)
//		return;
//
//	SubMesh            mDebugMeshLine;
//	SubMesh            mDebugMesh;
//	if (mMainDirLight && mMainDirLight->mCastShadow)
//	{
//		LFrustum f = LFrustum::FromOrth(0.01, 50, 30, 30);
//		f.Multiple(mMainDirLight->mViewMatrix.inverse());
//		AddLineToSubMesh(f.mNearPlane[0], f.mNearPlane[1], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[1], f.mNearPlane[2], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[2], f.mNearPlane[3], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[3], f.mNearPlane[0], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[0], f.mFarPlane[0], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[1], f.mFarPlane[1], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[2], f.mFarPlane[2], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[3], f.mFarPlane[3], mDebugMeshLine);
//		AddLineToSubMesh(f.mFarPlane[0], f.mFarPlane[1], mDebugMeshLine);
//		AddLineToSubMesh(f.mFarPlane[1], f.mFarPlane[2], mDebugMeshLine);
//		AddLineToSubMesh(f.mFarPlane[2], f.mFarPlane[3], mDebugMeshLine);
//		AddLineToSubMesh(f.mFarPlane[3], f.mFarPlane[0], mDebugMeshLine);
//	}
//
//	for (int i = 0; i < mPointLights.size(); i++)
//	{
//		PointLight* light = mPointLights[i];
//		if (light->mCastShadow)
//		{
//			for (int faceIdx = 0; faceIdx < 6; faceIdx++)
//			{
//				LFrustum f = LFrustum::MakeFrustrum(light->mFov, light->mNear, light->mFar, light->mAspect);
//				f.Multiple(light->mViewMatrix[faceIdx].inverse());
//
//				AddLineToSubMesh(f.mNearPlane[0], f.mNearPlane[1], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[1], f.mNearPlane[2], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[2], f.mNearPlane[3], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[3], f.mNearPlane[0], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[0], f.mFarPlane[0], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[1], f.mFarPlane[1], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[2], f.mFarPlane[2], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[3], f.mFarPlane[3], mDebugMeshLine);
//				AddLineToSubMesh(f.mFarPlane[0], f.mFarPlane[1], mDebugMeshLine);
//				AddLineToSubMesh(f.mFarPlane[1], f.mFarPlane[2], mDebugMeshLine);
//				AddLineToSubMesh(f.mFarPlane[2], f.mFarPlane[3], mDebugMeshLine);
//				AddLineToSubMesh(f.mFarPlane[3], f.mFarPlane[0], mDebugMeshLine);
//			}
//
//		}
//		AddCubeWiredToSubMesh(mDebugMeshLine, light->mPosition, LVector3f(1, 1, 1), light->mColor);
//	}
//
//	mDebugMeshData.Init(&mDebugMesh);
//	mDebugMeshLineData.Init(&mDebugMeshLine);
//}

}
