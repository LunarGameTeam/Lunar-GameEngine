#pragma once
#include "Core/Math/Math.h"
#include "Core/Foundation/Container.h"
#include "Core/Asset/BinaryAsset.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIResource.h"


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
	LArray<BaseVertex> mVertexData;
	LArray<uint32_t> mIndexData;

	RHIResourcePtr mVB;
	RHIResourcePtr mIB;
	RHIVertexLayout mVeretexLayout;
	//????mesh buffer????????buffer0???????pervertex??buffer1???????perinstance
	size_t GetStridePerVertex() { return mVeretexLayout.GetSize()[0]; };
	size_t GetStridePerInstance() { return mVeretexLayout.GetSize()[1]; };
private:
	bool m_ready = false;
};



class RENDER_API MeshAsset : public LBasicAsset
{
	RegisterTypeEmbedd(MeshAsset, Asset)
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
	TPPtrArray<SubMesh> mSubMesh;

	void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override;

	void OnAssetFileWrite(LSharedPtr<Dictionary> meta, LVector<byte>& data) override;
private:
	bool m_ready = false;
};
}
