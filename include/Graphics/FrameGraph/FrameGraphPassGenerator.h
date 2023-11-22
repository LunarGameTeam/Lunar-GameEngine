#pragma once
#include "Graphics/FrameGraph/FrameGraphNode.h"
namespace luna::graphics
{
	struct MeshDrawCommandBatch
	{
		LArray<RenderAssetDataMesh*> mRenderMeshs;
		LArray<size_t> mRoIndex;
		MaterialInstance* mMtl;
	};
	class FrameGraphPassGenerator
	{
	protected:

		LArray<RenderObject*> mRoQueue;

	public:
		FrameGraphPassGenerator() {}

		virtual void AddPassNode(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)  = 0;

		void FilterRenderObject(RenderObject* curRo);

		void ClearRoQueue();
	private:
		virtual bool CheckRenderObject(const RenderObject* curRo) const = 0;
	};
	
	void FrameGraphPassGenerator::FilterRenderObject(RenderObject* curRo)
	{
		if(CheckRenderObject(curRo))
		{
			mRoQueue.push_back(curRo);
		}
	}

	void FrameGraphPassGenerator::ClearRoQueue()
	{
		mRoQueue.clear();
	}

	class FrameGraphMeshPassGenerator : public FrameGraphPassGenerator
	{
	public:
		FrameGraphMeshPassGenerator(const LString passName) :
			FrameGraphPassGenerator(passName) {};

	};
}

