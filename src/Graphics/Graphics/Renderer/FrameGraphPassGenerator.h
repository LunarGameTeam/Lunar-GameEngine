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


struct PerViewMeshDrawDataMember
{
	RHIResourcePtr mRoIndexBuffer;
	RHIViewPtr     mRoIndexBufferView;
};

struct RenderObjectDrawBatch
{
public:
	int mID = 0;

	LUnorderedMap<size_t, PerViewMeshDrawDataMember>passData;
	RHIView* GetRoIndexBufferViewByPass(size_t passIndex);

	LUnorderedMap<MeshDrawCommandHashKey, MeshDrawCommandBatch, MeshDrawCommandHash> mAllCommandsPool;

	LUnorderedMap<MeshDrawCommandHashKey, LArray<size_t>, MeshDrawCommandHash> mAllRoIndexPool;

	MaterialInstanceGraphBase* mOverrideMaterial = nullptr;

	MaterialInstanceGraphBase* GetRenderObjectMaterial(const RenderObject* curRo)
	{
		if (mOverrideMaterial)
			return mOverrideMaterial;
		const RenderMeshBase* meshData = curRo->GetReadOnlyData<RenderMeshBase>();
		return meshData->mMaterial.Get();
	}

	void SetRenderObjects(RenderView* view, const LArray<RenderObject*>& mRoQueue);
	void DrawCommandBatch(RHICmdList* cmdList);
private:
	void GenerateRenderObjectIndexBuffer(void* pointer);
};

}


