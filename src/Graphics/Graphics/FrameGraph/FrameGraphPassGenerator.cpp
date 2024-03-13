#include "Graphics/FrameGraph/FrameGraphPassGenerator.h"
#include "Graphics/RenderModule.h"
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
namespace luna::graphics
{
	void FrameGraphPassGeneratorPerView::FilterRenderObject(RenderView* curView)
	{
		LArray<RenderObject*>& allObjects = curView->GetViewVisibleROs();
		for (RenderObject* eachRo : allObjects)
		{
			if (CheckRenderObject(eachRo))
			{
				mRoQueue.push_back(eachRo);
			}
		}
	}

	void FrameGraphPassGeneratorPerView::ClearRoQueue()
	{
		mRoQueue.clear();
	}

	RHIView* PerViewMeshDrawPassData::GetRoIndexBufferViewByPass(size_t passIndex)
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
			itor->second.mRoIndexBuffer = sRenderModule->GetRenderContext()->CreateBuffer(RHIHeapType::Default, desc);

			ViewDesc viewDesc;
			viewDesc.mViewType = RHIViewType::kStructuredBuffer;
			viewDesc.mViewDimension = RHIViewDimension::BufferView;
			viewDesc.mStructureStride = sizeof(uint32_t);
			itor->second.mRoIndexBufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
			itor->second.mRoIndexBufferView->BindResource(itor->second.mRoIndexBuffer);
		}
		return itor->second.mRoIndexBufferView;
	}

	FrameGraphMeshPassGenerator::FrameGraphMeshPassGenerator()
	{

	};

	void FrameGraphMeshPassGenerator::GenerateRenderObjectIndexBuffer(void* pointer)
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

	void FrameGraphMeshPassGenerator::FilterRenderObject(RenderView* curView)
	{
		FrameGraphPassGeneratorPerView::FilterRenderObject(curView);
		if (mRoQueue.size() == 0)
		{
			return;
		}
		PerViewMeshDrawPassData* curData = curView->RequireData<PerViewMeshDrawPassData>();
		for (auto& eachCommand : mAllCommandsPool)
		{
			//�������command cache��dp��Ϣ
			eachCommand.second.mDrawCount = 0;
		}
		for (auto& eachRoIndex : mAllRoIndexPool)
		{
			//�������command cache��ro index��Ϣ
			eachRoIndex.second.clear();
		}
		//��ʼ����ǰpass����Ⱦ����������Ⱦָ��
		for (auto eachDrawObj : mRoQueue)
		{
			MeshDrawCommandHashKey newKey;
			newKey.mMtl = SetMaterialByRenderObject(eachDrawObj);
			const RenderMeshBase* meshDataPointer = eachDrawObj->GetReadOnlyData<RenderMeshBase>();
			newKey.mRenderMeshs = meshDataPointer->mMeshData;
			//��¼ÿ��command��dp��Ϣ
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
			//��¼ÿ��command��Ӧ��ro id��
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
		//����index���µ�stage buffer
		RHIView* roIndexStageBuffer = curView->mOwnerScene->GetStageBufferPool()->AllocStructStageBuffer(
			mRoQueue.size() * sizeof(uint32_t),
			RHIViewType::kStructuredBuffer,
			sizeof(uint32_t),
			std::bind(&FrameGraphMeshPassGenerator::GenerateRenderObjectIndexBuffer, this, std::placeholders::_1)
		);
		//��������ָ�����ro index
		RHIView* roIndexBuffer = curData->GetRoIndexBufferViewByPass(mID);
		GpuSceneUploadCopyCommand* copyCommand = curView->mOwnerScene->AddCopyCommand();
		copyCommand->mStorageBufferOutput = roIndexBuffer->mBindResource;
		copyCommand->mUniformBufferInput = roIndexStageBuffer->mBindResource;
		copyCommand->mSrcOffset = 0;
		copyCommand->mDstOffset = 0;
		copyCommand->mCopyLength = mRoQueue.size() * sizeof(uint32_t);
	}

	void FrameGraphMeshPassGenerator::DrawCommandBatch(RHICmdList* cmdList)
	{
		LUnorderedSet<MaterialInstanceGraphBase*> materialUpdated;
		for (auto& eachCommand : mAllCommandsPool)
		{
			if (materialUpdated.find(eachCommand.second.mDrawParameter.mMtl) == materialUpdated.end())
			{
				eachCommand.second.mDrawParameter.mMtl->UpdateBindingSet();
				materialUpdated.insert(eachCommand.second.mDrawParameter.mMtl);
			}
			sRenderModule->GetRenderCommandHelper()->DrawMeshBatch(cmdList,eachCommand.second);
		}
	}
}
