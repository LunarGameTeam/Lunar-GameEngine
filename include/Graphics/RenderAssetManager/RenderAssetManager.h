#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Core/Foundation/Container.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"

namespace luna::graphics
{
	class SubMesh;
	class RenderAssetData : public HoldIdItem
	{
	};

	class RenderAssetDataMesh : public RenderAssetData
	{
		//几何体数据
		size_t            mVertexSize = 0;
		RHIResourcePtr    mVB;
		size_t            mIndexSize = 0;
		RHIResourcePtr    mIB;
		RHIVertexLayout   mVeretexLayout;
	public:
		size_t GetStridePerVertex() { return mVeretexLayout.GetSize()[0]; };
		size_t GetStridePerInstance() { return mVeretexLayout.GetSize()[1]; };
		size_t GetVertexSize() { return mVertexSize; };
		size_t GetIndexSize() { return mIndexSize; };

		RHIResource* GetVertexBuffer() { return mVB.get(); };
		RHIResource* GetIndexBuffer() { return mIB.get(); };
		RHIVertexLayout& GetVertexLayout() { return mVeretexLayout; };
		void Init(SubMesh* meshData);
		RenderAssetDataMesh()
		{
		}
	};

	class RenderAssetDataManager
	{
		LHoldIdArray<RenderAssetDataMesh> mAllRenderMesh;

		RenderAssetDataMesh* mFullScreenRenderMesh = nullptr;

		RenderAssetDataMesh* mDebugMeshLineData = nullptr;

		RenderAssetDataMesh* mDebugMeshData = nullptr;
	public:
		RenderAssetDataManager() {};

		RenderAssetDataMesh* GenerateRenderMesh(SubMesh* meshData);

		void DestroyRenderMesh(RenderAssetDataMesh* newData);

		RenderAssetDataMesh* GetFullScreenMesh();

		RenderAssetDataMesh* GetDebugMeshLineMesh();

		RenderAssetDataMesh* GetDebugMesh();
	};

}
