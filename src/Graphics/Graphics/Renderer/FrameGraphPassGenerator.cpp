#include "Graphics/Renderer/FrameGraphPassGenerator.h"
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
#include "Graphics/RHI/RhiUtils/RHIResourceGenerateHelper.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/Renderer/RenderContext.h"
namespace luna::graphics
{
RHIView* RenderObjectDrawBatch::GetRoIndexBufferViewByPass(size_t passIndex)
{
	auto itor = passData.find(passIndex);
	if (itor == passData.end())
	{
		passData.insert({ passIndex ,PerViewMeshDrawDataMember() });
		itor = passData.find(passIndex);
		size_t elementSize = sizeof(uint32_t);
		RHIBufferDesc desc;
		desc.mBufferUsage = RHIBufferUsage::StructureBuffer;
		desc.mSize = CommonSize128K;
		itor->second.mRoIndexBuffer = sGlobelRhiResourceGenerator->GetDeviceResourceGenerator()->CreateBuffer(RHIHeapType::Default, desc);

		ViewDesc viewDesc;
		viewDesc.mViewType = RHIViewType::kStructuredBuffer;
		viewDesc.mViewDimension = RHIViewDimension::BufferView;
		viewDesc.mStructureStride = sizeof(uint32_t);
		itor->second.mRoIndexBufferView = sGlobelRenderDevice->CreateView(viewDesc);
		itor->second.mRoIndexBufferView->BindResource(itor->second.mRoIndexBuffer);
	}
	return itor->second.mRoIndexBufferView;
}


void RenderObjectDrawBatch::GenerateRenderObjectIndexBuffer(void* pointer)
{
	uint32_t* indexPointer = (uint32_t*)pointer;
	size_t globelOffset = 0;
	for (auto& eachCommand : mAllCommandsPool)
	{
		if (eachCommand.second.mDrawCount == 0)
		{
			continue;
		}
		eachCommand.second.mRoOffset = globelOffset;

		LArray<size_t>& mRoIndexPool = mAllRoIndexPool[eachCommand.first];
		assert(mRoIndexPool.size() == eachCommand.second.mDrawCount);
		for (size_t curRoIndex : mRoIndexPool)
		{
			indexPointer[globelOffset] = curRoIndex;
			globelOffset += 1;
		}
	}
}

void RenderObjectDrawBatch::SetRenderObjects(RenderView* curView, const LArray<RenderObject*>& mRoQueue)
{
	if (mRoQueue.size() == 0)
	{
		return;
	}
	for (auto& eachCommand : mAllCommandsPool)
	{
		//清空所有command cache的dp信息
		eachCommand.second.mDrawCount = 0;
	}
	for (auto& eachRoIndex : mAllRoIndexPool)
	{
		//清空所有command cache的ro index信息
		eachRoIndex.second.clear();
	}
	//开始整理当前pass的渲染对象，制作渲染指令
	for (auto eachDrawObj : mRoQueue)
	{
		MeshDrawCommandHashKey newKey;
		newKey.mMtl = GetRenderObjectMaterial(eachDrawObj);
		const RenderMeshBase* meshDataPointer = eachDrawObj->GetReadOnlyData<RenderMeshBase>();
		newKey.mRenderMeshs = meshDataPointer->mMeshData;
		//记录每个command的dp信息
		auto itor = mAllCommandsPool.find(newKey);
		if (itor == mAllCommandsPool.end())
		{
			MeshDrawCommandBatch newBatch;
			newBatch.mDrawParameter = newKey;
			newBatch.mDrawCount = 1;
			mAllCommandsPool.insert({ newKey ,newBatch });
		}
		else
		{
			itor->second.mDrawCount += 1;
		}
		//记录每个command对应的ro id表
		auto itorIndex = mAllRoIndexPool.find(newKey);
		if (itorIndex == mAllRoIndexPool.end())
		{
			LArray<size_t> newRoArray;
			newRoArray.push_back(eachDrawObj->mID);
			mAllRoIndexPool.insert({ newKey ,newRoArray });
		}
		else
		{
			itorIndex->second.push_back(eachDrawObj->mID);
		}
	}
	//生成index更新的stage buffer
	RHIView* roIndexStageBuffer = curView->mOwnerScene->GetStageBufferPool()->AllocStructStageBuffer(
		mRoQueue.size() * sizeof(uint32_t),
		RHIViewType::kStructuredBuffer,
		sizeof(uint32_t),
		std::bind(&RenderObjectDrawBatch::GenerateRenderObjectIndexBuffer, this, std::placeholders::_1)
	);
	//制作拷贝指令，更新ro index
	RHIView* roIndexBuffer = GetRoIndexBufferViewByPass(mID);
	GpuSceneUploadCopyCommand* copyCommand = curView->mOwnerScene->AddCopyCommand();
	copyCommand->mStorageBufferOutput = roIndexBuffer->mBindResource;
	copyCommand->mUniformBufferInput = roIndexStageBuffer->mBindResource;
	copyCommand->mSrcOffset = 0;
	copyCommand->mDstOffset = 0;
	copyCommand->mCopyLength = mRoQueue.size() * sizeof(uint32_t);
}

void RenderObjectDrawBatch::DrawCommandBatch(RHICmdList* cmdList)
{
	LUnorderedSet<MaterialInstanceGraphBase*> materialUpdated;
	for (auto& eachCommand : mAllCommandsPool)
	{
		if (materialUpdated.find(eachCommand.second.mDrawParameter.mMtl) == materialUpdated.end())
		{
			eachCommand.second.mDrawParameter.mMtl->UpdateBindingSet();
			materialUpdated.insert(eachCommand.second.mDrawParameter.mMtl);
		}
		sRenderDrawContext->DrawMeshBatch(cmdList, eachCommand.second);
	}
}
}
