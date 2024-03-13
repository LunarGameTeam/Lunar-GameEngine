#pragma once
#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/RenderAssetManager/RenderAssetManager.h"
#include "Graphics/Renderer/RenderData.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderContext.h"
namespace luna::graphics
{
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

		virtual ~FrameGraphPassGeneratorPerView() {}

		virtual void AddPassNode(FrameGraphBuilder* builder, RenderView* view) = 0;

		virtual void FilterRenderObject(RenderView* curView);

		void ClearRoQueue();

		virtual const size_t GetRenderQueueSize() const { return mRoQueue.size(); };
	protected:
		virtual bool CheckRenderObject(const RenderObject* curRo) const = 0;
	};
	
	struct PerViewMeshDrawDataMember
	{
		RHIResourcePtr mRoIndexBuffer;
		RHIViewPtr     mRoIndexBufferView;
	};

	class PerViewMeshDrawPassData : public RenderData
	{
		LUnorderedMap<size_t, PerViewMeshDrawDataMember> passData;
	public:
		RHIView* GetRoIndexBufferViewByPass(size_t passIndex);
	};
	
	class FrameGraphMeshPassGenerator : public FrameGraphPassGeneratorPerView
	{
	protected:
		LUnorderedMap<MeshDrawCommandHashKey, MeshDrawCommandBatch, MeshDrawCommandHash> mAllCommandsPool;

		LUnorderedMap<MeshDrawCommandHashKey, LArray<size_t>, MeshDrawCommandHash> mAllRoIndexPool;
	public:
		FrameGraphMeshPassGenerator();

		virtual ~FrameGraphMeshPassGenerator() {};

		void FilterRenderObject(RenderView* curView) override;
	protected:

		virtual MaterialInstanceGraphBase* SetMaterialByRenderObject(const RenderObject* curRo) const = 0;

		void GenerateRenderObjectIndexBuffer(void* pointer);

		void DrawCommandBatch(RHICmdList* cmdList);
	};
}

