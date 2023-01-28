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
	static RHIVertexLayout& GetVertexLayout()
	{
		RHIVertexLayout layout;
		layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 3, 0, VertexElementInstanceType::PerVertex);
		layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageColor, 4, 0, VertexElementInstanceType::PerVertex);
		layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageNormal, 3, 0, VertexElementInstanceType::PerVertex);
		layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTangent, 4, 0, VertexElementInstanceType::PerVertex);

		layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture0, 2, 0, VertexElementInstanceType::PerVertex);
		layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture1, 2, 0, VertexElementInstanceType::PerVertex);
		layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture2, 2, 0, VertexElementInstanceType::PerVertex);
		layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture3, 2, 0, VertexElementInstanceType::PerVertex);
		return layout;
	}
};


class RENDER_API SubMesh : public LObject
{
	RegisterTypeEmbedd(SubMesh, LObject)
public:
	SubMesh();
	~SubMesh() override { Release(); }

	void ClearVertexData();
	
	void AddLine(const BaseVertex& v1, const BaseVertex& v2);
	void AddCubeWired(const LVector3f& position, const LVector3f& size = LVector3f(1, 1, 1), const LVector4f& color = LVector4f(1, 1, 1));
	void AddTriangle(const BaseVertex& v1, const BaseVertex& v2, const BaseVertex& v3);

	void Release();
	void Update();
	
	void SetVertexFormat(const RHIVertexLayout& format);
	RHIVertexLayout& GetVertexLayout() { return mVeretexLayout;	};

	uint32_t GetVertexCount() const { return (uint32_t)mVertexData.size(); }
	uint32_t GetIndexCount() const { return(uint32_t)mIndexData.size(); }
	int32_t mVertexCount;
	int32_t mIndexCount;
	LArray<BaseVertex> mVertexData;
	LArray<uint32_t> mIndexData;

	RHIResourcePtr mVB;
	RHIResourcePtr mIB;
	RHIVertexLayout mVeretexLayout;

	size_t GetStridePerVertex() { return mVeretexLayout.GetSize()[0]; };
	size_t GetStridePerInstance() { return mVeretexLayout.GetSize()[1]; };
private:
	bool mReady = false;
};



class RENDER_API MeshAsset : public Asset
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

	//Asset资源读入到内存时回调
	virtual void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;
	//Asset资源写入到磁盘时回调	
	virtual void OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data) override;
private:
	bool mReady = false;
};
}
