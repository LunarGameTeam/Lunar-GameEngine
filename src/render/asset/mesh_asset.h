#pragma once
#include "core/math/math.h"
#include "core/misc/container.h"
#include "core/asset/binary_asset.h"
#include "render/pch.h"
#include "render/rhi/rhi_resource.h"


namespace luna::render
{
class RHIResource;


struct BaseVertex
{
	LVector3f pos;
	LVector4f color;
	LVector3f normal;
	LVector4f tangent;
	LVector2f uv[4];
};


class RENDER_API SubMesh : public LObject
{
	RegisterTypeEmbedd(SubMesh, LObject)
public:
	SubMesh() = default;
	~SubMesh() override { Release(); }
	void Release();
	void Update();
	void Init();
	
	int32_t mVertexCount;
	int32_t mIndexCount;
	LVector<BaseVertex> mVertexData;
	LVector<uint32_t> mIndexData;

	RHIResourcePtr mVB;
	RHIResourcePtr mIB;
	InputVertexLayout mVeretexLayout;
private:
	bool m_ready = false;
};



class RENDER_API MeshAsset : public LBinaryWithHeadAsset
{
	RegisterTypeEmbedd(MeshAsset, LBasicAsset)
public:
	MeshAsset():
		mSubMesh(this)
	{
	}
	size_t SubMeshSize()
	{
		return mSubMesh.Size();
	}

	SubMesh* GetSubMeshAt(size_t index)
	{
		return mSubMesh[index];
	};
	TSubPtrArray<SubMesh> mSubMesh;
private:
	bool m_ready = false;
	void OnAssetBinaryRead(const byte* value) override;

	void OnAssetBinaryWrite(LVector<byte>& data) override;
};
}
