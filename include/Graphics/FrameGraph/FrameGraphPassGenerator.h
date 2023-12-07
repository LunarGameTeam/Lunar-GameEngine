#pragma once
#include "Graphics/FrameGraph/FrameGraphNode.h"
namespace luna::graphics
{
	struct MeshDrawCommandHashKey
	{
		RenderAssetDataMesh* mRenderMeshs = nullptr;
		MaterialInstanceGraphBase* mMtl = nullptr;
		bool operator==(MeshDrawCommandHashKey& key2) {
			if (mRenderMeshs->mID != key2.mRenderMeshs->mID)
			{
				return false;
			}
			if (mMtl != key2.mMtl)
			{
				return false;
			}
			return true;
		}
	};

	struct MeshDrawCommandBatch
	{
		MeshDrawCommandHashKey mDrawParameter;
		size_t mRoOffset = 0;
		size_t mDrawCount = 0;
	};

	class MeshDrawCommandHash
	{
	public:
		size_t operator()(const MeshDrawCommandHashKey& t) const
		{
			t.mMtl->GetInstanceID();
			return (t.mRenderMeshs->mID << 24) + t.mMtl->GetInstanceID();
		}
	};

	class FrameGraphPassGeneratorPerView : public HoldIdItem
	{
	protected:

		LArray<RenderObject*> mRoQueue;

	public:
		FrameGraphPassGeneratorPerView() {}

		virtual void AddPassNode(FrameGraphBuilder* builder, RenderView* view) = 0;

		virtual void FilterRenderObject(RenderView* curView);

		void ClearRoQueue();
	private:
		virtual bool CheckRenderObject(const RenderObject* curRo) const = 0;
	};
	
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


	struct PerViewMeshDrawDataMember
	{
		RHIResourcePtr mRoIndexBuffer;
		RHIViewPtr     mRoIndexBufferView;
	};

	class PerViewMeshDrawPassData : public RenderData
	{
		LUnorderedMap<size_t, PerViewMeshDrawDataMember> passData;
	public:
		RHIView* GetDataByPass(size_t passIndex);
	};
	
	RHIView* PerViewMeshDrawPassData::GetDataByPass(size_t passIndex)
	{
		auto itor = passData.find(passIndex);
		if (itor == passData.end())
		{
			passData.insert({ passIndex ,PerViewMeshDrawDataMember()});
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

	class FrameGraphMeshPassGenerator : public FrameGraphPassGeneratorPerView
	{
		LUnorderedMap<MeshDrawCommandHashKey,MeshDrawCommandBatch, MeshDrawCommandHash> mAllCommandsPool;


	public:
		FrameGraphMeshPassGenerator();
		void FilterRenderObject(RenderView* curView) override;
	private:
		virtual MaterialInstanceGraphBase* SetMaterialByRenderObject(const RenderObject* curRo) const = 0;
	};
	FrameGraphMeshPassGenerator::FrameGraphMeshPassGenerator()
	{

	};
	void FrameGraphMeshPassGenerator::FilterRenderObject(RenderView* curView)
	{
		FrameGraphPassGeneratorPerView::FilterRenderObject(curView);
		curView->RequireData<>();
		for (auto& eachCommand : mAllCommandsPool)
		{
			eachCommand.second.mDrawCount = 0;
		}
		for (auto eachDrawObj : mRoQueue)
		{
			MeshDrawCommandHashKey newKey;
			newKey.mMtl = SetMaterialByRenderObject(eachDrawObj);
			const RenderMeshBase* meshDataPointer = eachDrawObj->GetReadOnlyData<RenderMeshBase>();
			newKey.mRenderMeshs = meshDataPointer->mMeshData;
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
		}
		RHIView* lightDataBuffer = renderScene->GetStageBufferPool()->AllocStructStageBuffer(
			mDirtyList.size() * sizeof(LVector4f) * 4,
			RHIViewType::kStructuredBuffer,
			sizeof(LVector4f),
			std::bind(&PointBasedRenderLightData::GenerateDirtyLightDataBuffer, this, std::placeholders::_1)
		);
		for (auto& eachCommand : mAllCommandsPool)
		{
			if (eachCommand.second.mDrawCount == 0)
			{
				continue;
			}
			eachCommand.second.mDrawCount = 0;
		}
	}
}

